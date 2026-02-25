#include "config.h"
#include "logic.h"
#include "ui.h"

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <string>

// ANSI helpers
static const std::string RESET  = "\033[0m";
static const std::string BOLD   = "\033[1m";
static const std::string DIM    = "\033[2m";
static const std::string RED    = "\033[1;31m";
static const std::string GREEN  = "\033[1;32m";
static const std::string YELLOW = "\033[1;33m";

static void clearInputError() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int main() {
    // Load data
    auto dt = logic::loadAndCleanData(config::NAMA_FILE_EXCEL);
    const auto& columns = dt.columns;

    // Show header
    ui::tampilkanHeader();

    std::cout << "---------------------------------\n";
    std::cout << "Masukkan nomor pilihan: ";
    std::string pilihanMenu;
    std::getline(std::cin, pilihanMenu);

    if (pilihanMenu == "1") {
        while (true) {
            // --- Pilih kolom basis ---
            std::string kolomPencarian;
            double nilaiInput = 0.0;

            while (true) {
                std::cout << "\n" << BOLD << "Silakan pilih kolom basis pencarian:" << RESET << "\n";
                ui::tampilkanPilihKolom(columns);

                std::cout << "Masukkan nomor pilihan (misal: 1): ";
                std::string pilihanStr;
                std::getline(std::cin, pilihanStr);

                int pilihanInt = 0;
                try {
                    pilihanInt = std::stoi(pilihanStr);
                } catch (...) {
                    std::cout << "\n" << RED << "Error: Input tidak valid." << RESET << " Coba lagi.\n\n";
                    continue;
                }

                if (pilihanInt < 1 || pilihanInt > static_cast<int>(columns.size())) {
                    std::cout << "\n" << RED << "Error: Pilihan di luar jangkauan." << RESET << " Coba lagi.\n\n";
                    continue;
                }

                kolomPencarian = columns[pilihanInt - 1];
                std::cout << "Anda memilih: " << BOLD << GREEN << kolomPencarian << RESET << "\n";

                std::cout << "Masukkan nilai untuk " << kolomPencarian << ": ";
                std::string nilaiStr;
                std::getline(std::cin, nilaiStr);

                try {
                    nilaiInput = std::stod(nilaiStr);
                } catch (...) {
                    std::cout << "\n" << RED << "Error: Input tidak valid." << RESET << " Coba lagi.\n\n";
                    continue;
                }

                break;  // Valid input, exit selection loop
            }

            // --- Cari batas data ---
            auto batas = logic::cariBatasData(dt, kolomPencarian, nilaiInput);

            if (batas.found) {
                ui::tampilkanHasilPencarian(kolomPencarian, batas.bawah, batas.atas);

                auto bawahVal = logic::getValueFromRow(batas.bawah, kolomPencarian);
                auto atasVal  = logic::getValueFromRow(batas.atas, kolomPencarian);

                bool needInterpolation = bawahVal.has_value() && atasVal.has_value() &&
                                         std::abs(bawahVal.value() - atasVal.value()) > 1e-15;

                if (needInterpolation) {
                    std::cout << "\n" << std::string(20, '-') << "\n";
                    std::cout << YELLOW << BOLD << "Apakah Anda ingin melakukan interpolasi? (y/n): " << RESET;
                    std::string pilihanInterp;
                    std::getline(std::cin, pilihanInterp);

                    while (pilihanInterp == "y" || pilihanInterp == "Y") {
                        // --- Pilih kolom target ---
                        std::cout << "\n" << BOLD << "Pilih kolom target untuk interpolasi:" << RESET << "\n";
                        ui::tampilkanPilihKolom(columns, kolomPencarian);

                        std::cout << "Masukkan nomor kolom target: ";
                        std::string pilihanYStr;
                        std::getline(std::cin, pilihanYStr);

                        int pilihanYInt = 0;
                        try {
                            pilihanYInt = std::stoi(pilihanYStr);
                        } catch (...) {
                            std::cout << "\n" << RED << "Error: Pilihan tidak valid. Harap masukkan nomor dari daftar." << RESET << "\n";
                            continue;
                        }

                        if (pilihanYInt < 1 || pilihanYInt > static_cast<int>(columns.size())) {
                            std::cout << "\n" << RED << "Error: Pilihan tidak valid. Harap masukkan nomor dari daftar." << RESET << "\n";
                            continue;
                        }

                        std::string yCol = columns[pilihanYInt - 1];
                        if (yCol == kolomPencarian) {
                            std::cout << RED << "Error: Tidak bisa menginterpolasi kolom yang sama dengan basis." << RESET << "\n";
                            continue;
                        }

                        // --- Hitung interpolasi ---
                        auto y1Opt = logic::getValueFromRow(batas.bawah, yCol);
                        auto y2Opt = logic::getValueFromRow(batas.atas, yCol);

                        if (!y1Opt.has_value() || !y2Opt.has_value()) {
                            std::cout << "\n" << RED << "Error: Kolom '" << yCol << "' berisi data non-numerik." << RESET << "\n";
                            continue;
                        }

                        double x1 = bawahVal.value();
                        double x2 = atasVal.value();
                        double y1 = y1Opt.value();
                        double y2 = y2Opt.value();

                        auto interpResult = logic::hitungInterpolasi(x1, x2, y1, y2, nilaiInput);

                        if (!interpResult.success) {
                            std::cout << RED << interpResult.error << RESET << "\n";
                        } else {
                            ui::tampilkanHasilInterpolasi(yCol, interpResult.value, nilaiInput,
                                                          x1, x2, y1, y2);
                        }

                        std::cout << "\n" << std::string(20, '-') << "\n";
                        std::cout << YELLOW << BOLD << "Apakah Anda ingin interpolasi kolom LAIN? (y/n): " << RESET;
                        std::getline(std::cin, pilihanInterp);
                    }
                }
            }

            std::cout << "\n" << std::string(40, '=') << "\n";
            std::cout << YELLOW << BOLD << "Apakah Anda ingin melakukan pencarian lagi? (y/n): " << RESET;
            std::string pilihanLanjut;
            std::getline(std::cin, pilihanLanjut);
            if (pilihanLanjut != "y" && pilihanLanjut != "Y") {
                break;
            }
            std::cout << "\n\n";
        }
    }

    std::cout << "\n" << DIM << "Terima kasih telah menggunakan InterpKit. Goodbye!" << RESET << "\n";
    return 0;
}
