#pragma once

#include "logic.h"
#include <string>
#include <vector>

namespace ui {

// Display the header with ASCII art logo and menu
void tampilkanHeader();

// Display search results (exact match or batas bawah/atas)
void tampilkanHasilPencarian(const std::string& kolomPencarian,
                              const logic::DataRow& bawah,
                              const logic::DataRow& atas);

// Display interpolation result
void tampilkanHasilInterpolasi(const std::string& yCol, double yTarget, double xTarget,
                                double x1, double x2, double y1, double y2);

// Display column selection menu and return column names
void tampilkanPilihKolom(const std::vector<std::string>& columns,
                          const std::string& basisColumn = "");

// Clear screen
void clearScreen();

} // namespace ui
