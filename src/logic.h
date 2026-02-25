#pragma once

#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace logic {

// Represents a single row of data as key-value pairs (column_name -> value)
using DataRow = std::vector<std::pair<std::string, double>>;

struct DataTable {
    std::vector<std::string> columns;
    std::vector<DataRow>     rows;
};

struct BatasResult {
    bool    found = false;
    DataRow bawah;
    DataRow atas;
};

struct InterpolasiResult {
    bool        success = false;
    double      value   = 0.0;
    std::string error;
};

// Load and clean data from an Excel file
DataTable loadAndCleanData(const std::string& filePath);

// Find batas bawah and batas atas for a given value in a column
BatasResult cariBatasData(const DataTable& dt, const std::string& searchColumn, double inputValue);

// Linear interpolation calculation
InterpolasiResult hitungInterpolasi(double x1, double x2, double y1, double y2, double xTarget);

// Helper: get value from a DataRow by column name
std::optional<double> getValueFromRow(const DataRow& row, const std::string& colName);

} // namespace logic
