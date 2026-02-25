#include "ui.h"
#include "config.h"
#include "logic.h"

#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/table.hpp>
#include <ftxui/screen/screen.hpp>

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace ui {

// ANSI color helpers
static const std::string RESET     = "\033[0m";
static const std::string BOLD      = "\033[1m";
static const std::string DIM       = "\033[2m";
static const std::string RED       = "\033[1;31m";
static const std::string GREEN     = "\033[1;32m";
static const std::string YELLOW    = "\033[1;33m";
static const std::string BLUE      = "\033[1;34m";
static const std::string CYAN      = "\033[36m";
static const std::string DEEP_SKY  = "\033[38;5;39m";

static std::string formatDouble(double val, int precision = 4) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(precision) << val;
    return oss.str();
}

// Compute terminal display width of a UTF-8 string,
// skipping zero-width Unicode codepoints (e.g. U+200B zero-width space).
static size_t displayWidth(const std::string& s) {
    size_t width = 0;
    size_t i = 0;
    const unsigned char* u = reinterpret_cast<const unsigned char*>(s.data());
    const size_t len = s.size();
    while (i < len) {
        uint32_t cp = 0;
        unsigned char c = u[i];
        int seqLen = 1;
        if      (c < 0x80)  { cp = c;                                seqLen = 1; }
        else if (c < 0xE0)  { cp = c & 0x1F;                        seqLen = 2; }
        else if (c < 0xF0)  { cp = c & 0x0F;                        seqLen = 3; }
        else                { cp = c & 0x07;                        seqLen = 4; }

        for (int k = 1; k < seqLen && (i + k) < len; k++) {
            cp = (cp << 6) | (u[i + k] & 0x3F);
        }
        i += seqLen;

        // Skip zero-width characters: U+200B, U+200C, U+200D, U+FEFF (BOM), etc.
        if (cp == 0x200B || cp == 0x200C || cp == 0x200D || cp == 0xFEFF ||
            cp == 0x00AD || cp == 0x034F || cp == 0x17B4 || cp == 0x17B5) {
            continue;  // zero-width, don't count
        }
        width++;
    }
    return width;
}

// Pad string to display width 'w' with trailing spaces
static std::string padToDisplay(const std::string& s, size_t w) {
    size_t dw = displayWidth(s);
    if (dw >= w) return s;
    return s + std::string(w - dw, ' ');
}

// Split a multi-line string into individual lines
static std::vector<std::string> splitLines(const std::string& s) {
    std::vector<std::string> lines;
    std::istringstream stream(s);
    std::string line;
    while (std::getline(stream, line)) {
        lines.push_back(line);
    }
    return lines;
}

// Convert multi-line string to vbox of text elements
static ftxui::Element multiLineText(const std::string& s, ftxui::Decorator dec = ftxui::nothing) {
    auto lines = splitLines(s);
    ftxui::Elements elements;
    for (auto& line : lines) {
        elements.push_back(ftxui::text(line) | dec);
    }
    return ftxui::vbox(std::move(elements));
}

// Render an FTXUI element to the terminal
static void renderElement(ftxui::Element element) {
    auto doc = element;
    auto screen = ftxui::Screen::Create(ftxui::Dimension::Fit(doc));
    ftxui::Render(screen, doc);
    screen.Print();
    std::cout << std::endl;
}

void clearScreen() {
    std::cout << "\033[2J\033[H" << std::flush;
}

void tampilkanHeader() {
    using namespace ftxui;

    clearScreen();

    std::string asciiArt =
        " /$$$$$$             /$$                                   /$$   /$$ /$$   /$$\n"
        " |_ $$_/            | $$                                  | $$  /$$/ |__/ | $$\n"
        "  | $$   /$$$$$$$  /$$$$$$    /$$$$$$   /$$$$$$   /$$$$$$ | $$ /$$/  /$$ /$$$$$$\n"
        "  | $$  | $$__  $$|_  $$_/   /$$__  $$ /$$__  $$ /$$__  $$| $$$$$/  | $$|_  $$_/\n"
        "  | $$  | $$  \\ $$  | $$    | $$$$$$$$| $$  \\__/| $$  \\ $$| $$  $$  | $$  | $$\n"
        "  | $$  | $$  | $$  | $$ /$$| $$_____/| $$      | $$  | $$| $$\\  $$ | $$  | $$ /$$\n"
        " /$$$$$$| $$  | $$  |  $$$$/|  $$$$$$$| $$      | $$$$$$$/| $$ \\  $$| $$  |  $$$$/\n"
        "|______/|__/  |__/   \\___/   \\_______/|__/      | $$____/ |__/  \\__/|__/   \\___/\n"
        "                                                | $$\n"
        "                                                | $$\n"
        "                                                |__/";

    // Left side: ASCII art logo
    auto logo = multiLineText(asciiArt, color(Color::DeepSkyBlue1));

    // Right side: welcome text + menu
    auto menu = vbox({
        text("Selamat datang di InterpKit!") | bold,
        text(""),
        text("Silakan pilih menu:"),
        text(""),
        hbox({text("1") | color(Color::Cyan), text(". Mulai Pencarian Data")}),
        hbox({text("2") | color(Color::Cyan), text(". Keluar")}),
    });

    // Footer
    auto footer = text(config::VERSI_PROGRAM + " | (c) 2025 Feby") | bold | dim | center;

    auto doc = window(
        text(config::NAMA_PROGRAM) | bold | center,
        vbox({
            hbox({
                logo,
                text("    "),  // spacing between logo and menu
                menu | vcenter,
            }),
            separator(),
            text(""),
            footer,
        })
    ) | color(Color::Blue);

    renderElement(doc);
}

void tampilkanPilihKolom(const std::vector<std::string>& columns,
                          const std::string& basisColumn) {
    for (size_t i = 0; i < columns.size(); i++) {
        if (!basisColumn.empty() && columns[i] == basisColumn) {
            std::cout << DIM << " " << (i + 1) << ". " << columns[i]
                      << " (Kolom Basis)" << RESET << "\n";
        } else {
            std::cout << " " << CYAN << (i + 1) << RESET << ". " << columns[i] << "\n";
        }
    }
    std::cout << "---------------------------------\n";
}

void tampilkanHasilPencarian(const std::string& kolomPencarian,
                              const logic::DataRow& bawah,
                              const logic::DataRow& atas) {
    using namespace ftxui;

    auto bawahVal = logic::getValueFromRow(bawah, kolomPencarian);
    auto atasVal  = logic::getValueFromRow(atas, kolomPencarian);

    bool exactMatch = bawahVal.has_value() && atasVal.has_value() &&
                      std::abs(bawahVal.value() - atasVal.value()) < 1e-15;

    // Pad a string to exactly 'width' chars with trailing spaces
    auto buildTable = [](const logic::DataRow& row) {
        // Collect raw data
        std::vector<std::string> col0 = {"Atribut"};
        std::vector<std::string> col1 = {"Nilai"};
        for (auto& [name, val] : row) {
            col0.push_back(name);
            col1.push_back(formatDouble(val));
        }

        // Compute max VISUAL width for each column (handles unicode zero-width chars)
        size_t w0 = 0, w1 = 0;
        for (auto& s : col0) w0 = std::max(w0, displayWidth(s));
        for (auto& s : col1) w1 = std::max(w1, displayWidth(s));

        // Build padded table data using visual width
        std::vector<std::vector<std::string>> tableData;
        for (size_t i = 0; i < col0.size(); i++) {
            tableData.push_back({padToDisplay(col0[i], w0), padToDisplay(col1[i], w1)});
        }

        auto table = Table(tableData);
        table.SelectAll().Border(LIGHT);
        table.SelectRow(0).Decorate(bold);
        table.SelectRow(0).SeparatorVertical(LIGHT);
        table.SelectAll().SeparatorVertical(LIGHT);
        table.SelectColumn(0).Decorate(color(Color::Cyan));
        return table.Render();
    };

    if (exactMatch) {
        auto doc = vbox({
            text(" Nilai Ditemukan Persis") | bold | color(Color::Green),
            buildTable(bawah) | color(Color::Green)
        });

        renderElement(doc);
    } else {
        std::string subtitleBawah = "Nilai: " + formatDouble(bawahVal.value_or(0));
        std::string subtitleAtas  = "Nilai: " + formatDouble(atasVal.value_or(0));

        auto doc = vbox({
            text(""),
            text(" Hasil Pencarian Batas Data:") | bold,
            text(""),
            hbox({
                vbox({
                    text(" BATAS BAWAH (" + subtitleBawah + ")") | bold | color(Color::Blue),
                    buildTable(bawah) | color(Color::Blue),
                }),
                text("   "),
                vbox({
                    text(" BATAS ATAS (" + subtitleAtas + ")") | bold | color(Color::Blue),
                    buildTable(atas) | color(Color::Blue),
                }),
            }),
        });

        renderElement(doc);
    }
}

void tampilkanHasilInterpolasi(const std::string& yCol, double yTarget, double xTarget,
                                double x1, double x2, double y1, double y2) {
    using namespace ftxui;

    auto doc = window(
        text("HASIL INTERPOLASI LINIER") | bold | color(Color::Green),
        vbox({
            text(""),
            hbox({text("  Interpolasi untuk "), text(yCol) | bold}),
            text(""),
            hbox({text("  X Target : "), text(formatDouble(xTarget, 6))}),
            hbox({text("  Y Hasil  : "), text(formatDouble(yTarget, 6)) | bold | color(Color::Green)}),
            text(""),
            text("  Berdasarkan:") | dim,
            text("   (x1: " + formatDouble(x1) + ", y1: " + formatDouble(y1) + ")") | dim,
            text("   (x2: " + formatDouble(x2) + ", y2: " + formatDouble(y2) + ")") | dim,
            text(""),
        })
    ) | color(Color::Green);

    renderElement(doc);
}

} // namespace ui
