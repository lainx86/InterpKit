#include "logic.h"
#include "config.h"
#include "embedded_data.h"

#include <OpenXLSX.hpp>
#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <stdexcept>

namespace logic {

// Trim whitespace from a string
static std::string trimString(const std::string& s) {
    auto start = s.begin();
    while (start != s.end() && std::isspace(static_cast<unsigned char>(*start)))
        ++start;
    auto end = s.end();
    while (end != start && std::isspace(static_cast<unsigned char>(*(end - 1))))
        --end;
    return std::string(start, end);
}

// Try to parse a string as double
static std::optional<double> tryParseDouble(const std::string& s) {
    if (s.empty()) return std::nullopt;
    try {
        size_t pos;
        double val = std::stod(s, &pos);
        if (pos == s.size()) return val;
        return std::nullopt;
    } catch (...) {
        return std::nullopt;
    }
}

// Write embedded xlsx data to a temp file and return the path
static std::string writeEmbeddedToTemp() {
    // Generate a temporary file name
    char tmpName[L_tmpnam];
    if (std::tmpnam(tmpName) == nullptr) {
        std::cerr << "Error: Tidak bisa membuat nama file temporary.\n";
        std::exit(1);
    }
    std::string tmpPath = std::string(tmpName) + ".xlsx";

    // Write data via standard output stream
    std::ofstream out(tmpPath, std::ios::binary);
    if (!out) {
        std::cerr << "Error: Tidak bisa membuka file temporary untuk ditulis.\n";
        std::exit(1);
    }

    out.write(reinterpret_cast<const char*>(embedded::XLSX_DATA), embedded::XLSX_DATA_SIZE);
    
    if (!out.good()) {
        std::cerr << "Error: Gagal menulis data ke file temporary.\n";
        out.close();
        std::exit(1);
    }
    
    out.close();
    return tmpPath;
}

DataTable loadAndCleanData(const std::string& /*filePath*/) {
    DataTable dt;

    // Write embedded data to temp file, load with OpenXLSX, then clean up
    std::string tmpPath = writeEmbeddedToTemp();

    OpenXLSX::XLDocument doc;
    try {
        doc.open(tmpPath);
    } catch (const std::exception& e) {
        std::remove(tmpPath.c_str());
        std::cerr << "Error: Gagal membuka data embedded.\n";
        std::cerr << "Detail: " << e.what() << "\n";
        std::exit(1);
    }

    auto wks = doc.workbook().worksheet(1);

    // Determine dimensions: find the used row/column count
    auto rowCount = wks.rowCount();
    auto colCount = wks.columnCount();

    if (rowCount == 0 || colCount == 0) {
        std::cerr << "Error: File tidak memiliki data yang valid.\n";
        std::exit(1);
    }

    // Read header row (row 1)
    for (uint16_t col = 1; col <= colCount; ++col) {
        OpenXLSX::XLCellValue cellVal = wks.cell(1, col).value();
        std::string header;
        if (cellVal.type() == OpenXLSX::XLValueType::String) {
            header = static_cast<std::string>(cellVal);
        } else if (cellVal.type() == OpenXLSX::XLValueType::Integer) {
            header = std::to_string(static_cast<int64_t>(cellVal));
        } else if (cellVal.type() == OpenXLSX::XLValueType::Float) {
            header = std::to_string(static_cast<double>(cellVal));
        } else {
            header = "Col" + std::to_string(col);
        }
        header = trimString(header);
        if (header.empty()) break;  // Stop at first empty column
        dt.columns.push_back(header);
    }

    // Read data rows (row 2 onwards)
    for (uint32_t row = 2; row <= rowCount; ++row) {
        DataRow dataRow;
        bool hasData = false;

        for (size_t colIdx = 0; colIdx < dt.columns.size(); ++colIdx) {
            uint16_t col = static_cast<uint16_t>(colIdx + 1);
            OpenXLSX::XLCellValue cellVal = wks.cell(row, col).value();
            double value = 0.0;

            if (cellVal.type() == OpenXLSX::XLValueType::Integer) {
                value = static_cast<double>(static_cast<int64_t>(cellVal));
                hasData = true;
            } else if (cellVal.type() == OpenXLSX::XLValueType::Float) {
                value = static_cast<double>(cellVal);
                hasData = true;
            } else if (cellVal.type() == OpenXLSX::XLValueType::String) {
                std::string strVal = static_cast<std::string>(cellVal);
                auto parsed = tryParseDouble(strVal);
                if (parsed.has_value()) {
                    value = parsed.value();
                    hasData = true;
                }
            }
            // Empty or other types default to 0.0

            dataRow.push_back({dt.columns[colIdx], value});
        }

        if (hasData) {
            dt.rows.push_back(std::move(dataRow));
        }
    }

    doc.close();
    std::remove(tmpPath.c_str());  // Clean up temp file

    if (dt.columns.empty() || dt.rows.empty()) {
        std::cerr << "Error: File tidak memiliki data yang valid.\n";
        std::exit(1);
    }

    return dt;
}

std::optional<double> getValueFromRow(const DataRow& row, const std::string& colName) {
    for (auto& [name, val] : row) {
        if (name == colName) return val;
    }
    return std::nullopt;
}

BatasResult cariBatasData(const DataTable& dt, const std::string& searchColumn, double inputValue) {
    BatasResult result;

    // Gather rows with valid values in the search column, sorted
    struct IndexedValue {
        size_t rowIndex;
        double value;
    };
    std::vector<IndexedValue> sortedRows;

    for (size_t i = 0; i < dt.rows.size(); i++) {
        auto val = getValueFromRow(dt.rows[i], searchColumn);
        if (val.has_value()) {
            sortedRows.push_back({i, val.value()});
        }
    }

    std::sort(sortedRows.begin(), sortedRows.end(),
              [](const IndexedValue& a, const IndexedValue& b) { return a.value < b.value; });

    // Find batas bawah (last row <= inputValue)
    int bawahIdx = -1;
    for (int i = static_cast<int>(sortedRows.size()) - 1; i >= 0; i--) {
        if (sortedRows[i].value <= inputValue) {
            bawahIdx = i;
            break;
        }
    }

    if (bawahIdx < 0) {
        std::cerr << "\033[1;31mError: Nilai input " << inputValue
                  << " lebih kecil dari data terkecil.\033[0m\n";
        return result;
    }

    // Find batas atas (first row >= inputValue)
    int atasIdx = -1;
    for (size_t i = 0; i < sortedRows.size(); i++) {
        if (sortedRows[i].value >= inputValue) {
            atasIdx = static_cast<int>(i);
            break;
        }
    }

    if (atasIdx < 0) {
        std::cerr << "\033[1;31mError: Nilai input " << inputValue
                  << " lebih besar dari data terbesar.\033[0m\n";
        return result;
    }

    result.found = true;
    result.bawah = dt.rows[sortedRows[bawahIdx].rowIndex];
    result.atas   = dt.rows[sortedRows[atasIdx].rowIndex];

    return result;
}

InterpolasiResult hitungInterpolasi(double x1, double x2, double y1, double y2, double xTarget) {
    InterpolasiResult res;

    if (std::abs(x2 - x1) < 1e-15) {
        res.success = false;
        res.error = "Error: Nilai batas atas dan bawah sama (pembagian dengan nol).";
        return res;
    }

    res.value = y1 + ((xTarget - x1) * (y2 - y1)) / (x2 - x1);
    res.success = true;
    return res;
}

} // namespace logic
