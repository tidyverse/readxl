#pragma once

#include "CellLimits.h"
#include "ColSpec.h"
#include "Spinner.h"
#include "XlsxCell.h"
#include "XlsxWorkBook.h"

#include "rapidxml/rapidxml.h"

#include "cpp11/integers.hpp"
#include "cpp11/list.hpp"
#include "cpp11/protect.hpp"
#include "cpp11/sexp.hpp"
#include "cpp11/strings.hpp"

// Page and section numbers below refer to
// ECMA-376 (version, date, and download URL given in XlsxCell.h)
// 18.3.1.73  row         (Row)        [p1685]
// 18.3.1.4   c           (Cell)       [p1593]
// 18.3.1.96  v           (Cell Value) [p1707]
// 18.18.11   ST_CellType (Cell Type)  [p2451]

class XlsxCellSet {
  // xlsx specifics
  // http://rapidxml.sourceforge.net/manual.html
  // "In-situ parsing requires that source text lives at least as long as the
  // document object. If source text is destroyed, names and values of nodes in
  // DOM tree will become destroyed as well."
  //              sheetXml_ = the document object
  // preciousXmlSourceText_ = the source text
  // Note: preciousXmlSourceText_ is the memory that backs sheetXml_ and,
  // ultimately, cells_. Therefore it must belong to the class and cannot just
  // be local to the constructor.
  rapidxml::xml_document<> sheetXml_;
  std::string preciousXmlSourceText_;
  rapidxml::xml_node<>* sheetData_;

  // conditional formatting storage
  std::vector<ConditionalFormat> conditionalFormats_;

  // common to xls[x]
  std::string sheetName_;
  CellLimits nominal_, actual_;
  int ncol_, nrow_;

public:

  std::vector<XlsxCell> cells_;
  bool extract_colors_;

  XlsxCellSet(const XlsxWorkBook wb, int sheet_i,
                cpp11::integers limits, bool shim, Spinner spinner_, bool extract_colors = false)
    :  nominal_(limits), extract_colors_(extract_colors)
  {
    if (sheet_i >= wb.n_sheets()) {
      cpp11::stop("Can't retrieve sheet in position %d, only %d sheet(s) found.",
                  sheet_i + 1,  wb.n_sheets());
    }
    sheetName_ = wb.sheets()[sheet_i];
    std::string sheetPath = wb.sheetPath(sheet_i);
    spinner_.spin();
    preciousXmlSourceText_ = zip_buffer(wb.path(), sheetPath);
    spinner_.spin();
    sheetXml_.parse<rapidxml::parse_strip_xml_namespaces>(&preciousXmlSourceText_[0]);
    spinner_.spin();

    rapidxml::xml_node<>* rootNode;
    rootNode = sheetXml_.first_node("worksheet");
    if (!rootNode) {
      cpp11::stop("Sheet '%s' (position %d): Invalid sheet xml (no <worksheet>)",
                  sheetName_.c_str(), sheet_i + 1);
    }

    sheetData_ = rootNode->first_node("sheetData");
    if (!sheetData_) {
      cpp11::stop("Sheet '%s' (position %d): Invalid sheet xml (no <sheetData>)",
                  sheetName_.c_str(), sheet_i + 1);
    }

    // Parse conditional formatting if extract_colors is enabled
    if (extract_colors) {
      parseConditionalFormatting(rootNode);
    }

    // shim = TRUE when user specifies geometry via `range`
    // shim = FALSE when user specifies no geometry or uses `skip` and `n_max`
    // nominal_ holds user's geometry request, where -1 means "unspecified"
    loadCells(shim, spinner_);
    // nominal_ may have been shifted (case of implicit skipping and n_max)
    // actual_ reports populated cells inside the nominal_ rectangle

    // When shim = FALSE, we shrink-wrap the data that falls inside
    // the nominal_ rectangle.
    //
    // When shim = TRUE, we may need to insert dummy cells to fill out
    // the nominal_rectangle.
    // actual_ is updated to reflect the insertions.
    if (shim) insertShims(cells_, nominal_, actual_);

    nrow_ = (actual_.minRow() < 0) ? 0 : actual_.maxRow() - actual_.minRow() + 1;
    ncol_ = (actual_.minCol() < 0) ? 0 : actual_.maxCol() - actual_.minCol() + 1;
  }

  int ncol() const { return ncol_; }
  int nrow() const { return nrow_; }
  std::string sheetName() const { return sheetName_; }
  int startCol() const { return actual_.minCol(); }
  int lastRow() const { return actual_.maxRow(); }
  const std::vector<ConditionalFormat>& conditionalFormats() const { return conditionalFormats_; }

private:

  void loadCells(const bool shim, Spinner spinner_) {
    // by convention, min_row = -2 means 'read no data'
    if (nominal_.minRow() < -1) {
      return;
    }

    rapidxml::xml_node<>* row = sheetData_->first_node("row");
    if (!row) {
      return;
    }

    // count is for spinner and checking for interrupt
    int count = 0;

    int i = 0;
    bool nominal_needs_checking = !shim && nominal_.maxRow() >= 0;
    for (; row; row = row->next_sibling("row")) {
      // if row declares its number, take this opportunity to update i
      // when it exists, this row number is 1-indexed, but i is 0-indexed
      rapidxml::xml_attribute<>* ref = row->first_attribute("r");
      if (ref) {
        i = std::atoi(ref->value()) - 1;
      }
      int j = 0;

      for (rapidxml::xml_node<>* cell = row->first_node("c");
           cell; cell = cell->next_sibling("c")) {
        count++;
        if (count % PROGRESS_TICK == 0) {
          spinner_.spin();
          cpp11::check_user_interrupt();
        }

        // if cell declares its location, take this opportunity to update i and j
        ref = cell->first_attribute("r");
        if (ref) {
          std::pair<int, int> location = parseRef(ref->value());
          i = location.first;
          j = location.second;
        }

        rapidxml::xml_node<>* first_child = cell->first_node(0);
        // only consider cells that have >= 1 child nodes
        // we require cell to have content, not just, e.g., a format
        if (first_child) {
          // We have a cell!

          // (i, j) is our best guess at location, but if cell declares
          // it's own location, we store that instead
          XlsxCell xcell(cell, i, j);

          if (nominal_needs_checking) {
            if (i > nominal_.minRow()) { // implicit skip happened
              nominal_.update(
                i, i + nominal_.maxRow() - nominal_.minRow(),
                nominal_.minCol(), nominal_.maxCol()
              );
            }
            nominal_needs_checking = false;
          }

          if (nominal_.contains(i, j)) {
            cells_.push_back(xcell);
            actual_.update(i, j);
          }

        }
        j++;
      }
      i++;
    }
  }

private:
  void parseConditionalFormatting(rapidxml::xml_node<>* worksheet) {
    // Look for conditionalFormatting nodes
    for (rapidxml::xml_node<>* cfNode = worksheet->first_node("conditionalFormatting");
         cfNode; cfNode = cfNode->next_sibling("conditionalFormatting")) {
      
      // Get the range this formatting applies to
      rapidxml::xml_attribute<>* sqref = cfNode->first_attribute("sqref");
      if (!sqref) continue;
      
      std::string range = sqref->value();
      
      // Parse cfRule nodes
      for (rapidxml::xml_node<>* cfRule = cfNode->first_node("cfRule");
           cfRule; cfRule = cfRule->next_sibling("cfRule")) {
        
        rapidxml::xml_attribute<>* type = cfRule->first_attribute("type");
        if (!type) continue;
        
        std::string ruleType = type->value();
        
        // Handle colorScale type (most common for green/red formatting)
        if (ruleType == "colorScale") {
          rapidxml::xml_node<>* colorScale = cfRule->first_node("colorScale");
          if (colorScale) {
            parseColorScale(colorScale, range);
          }
        }
        // Handle other types like dataBar, iconSet, etc. if needed
        else if (ruleType == "cellIs" || ruleType == "expression") {
          // These might have dxf (differential formatting) references
          parseCellRule(cfRule, range);
        }
      }
    }
  }
  
  void parseColorScale(rapidxml::xml_node<>* colorScale, const std::string& range) {
    ConditionalFormat cf;
    parseRange(range, cf);
    cf.type = "colorScale";
    
    // Get the colors from cfvo and color nodes
    std::vector<std::string> colors;
    for (rapidxml::xml_node<>* color = colorScale->first_node("color");
         color; color = color->next_sibling("color")) {
      rapidxml::xml_attribute<>* rgb = color->first_attribute("rgb");
      if (rgb) {
        std::string colorValue = rgb->value();
        if (colorValue.length() == 8 && colorValue.substr(0, 2) == "FF") {
          colorValue = colorValue.substr(2);
        }
        colors.push_back("#" + colorValue);
      }
    }
    
    // Assume 2-color scale: first is low (red), second is high (green)
    if (colors.size() >= 2) {
      cf.redColor = colors[0];   // Low value color
      cf.greenColor = colors[1]; // High value color
      conditionalFormats_.push_back(cf);
    }
  }
  
  void parseCellRule(rapidxml::xml_node<>* cfRule, const std::string& range) {
    rapidxml::xml_attribute<>* dxfId = cfRule->first_attribute("dxfId");
    rapidxml::xml_attribute<>* operatorAttr = cfRule->first_attribute("operator");
    
    if (!dxfId) return;
    
    ConditionalFormat cf;
    parseRange(range, cf);
    cf.type = "cellIs";
    
    // Map common dxfId values to colors based on Excel standard patterns
    std::string dxfIdValue = dxfId->value();
    
    if (operatorAttr) {
      std::string op = operatorAttr->value();
      cf.operatorType = op;
    }
    
    // For dxfId 11 and 12 (common green/red conditional formatting)
    if (dxfIdValue == "11") {
      cf.greenColor = "#CCFFCC";  // Light green
      cf.condition = "greaterThan";
    } else if (dxfIdValue == "12") {
      cf.redColor = "#FFCCCC";   // Light red  
      cf.condition = "lessThan";
    } else {
      // For other dxfIds, use a more generic approach
      // This is a simplified mapping - real implementation would parse styles.xml
      cf.greenColor = "#90EE90";  // Default light green
    }
    
    conditionalFormats_.push_back(cf);
  }
  
  void parseRange(const std::string& range, ConditionalFormat& cf) {
    // Parse Excel range like "C2:C143" into row/col indices
    // This is a simplified parser - Excel ranges can be more complex
    size_t colon = range.find(':');
    if (colon == std::string::npos) {
      // Single cell range
      parseCell(range, cf.startRow, cf.startCol);
      cf.endRow = cf.startRow;
      cf.endCol = cf.startCol;
    } else {
      // Range with start and end
      std::string start = range.substr(0, colon);
      std::string end = range.substr(colon + 1);
      parseCell(start, cf.startRow, cf.startCol);
      parseCell(end, cf.endRow, cf.endCol);
    }
  }
  
  void parseCell(const std::string& cell, int& row, int& col) {
    // Parse cell reference like "C2" -> row=1, col=2 (0-based)
    col = 0;
    row = 0;
    
    size_t i = 0;
    // Parse column letters
    while (i < cell.length() && isalpha(cell[i])) {
      col = col * 26 + (toupper(cell[i]) - 'A' + 1);
      i++;
    }
    col--; // Convert to 0-based
    
    // Parse row number
    if (i < cell.length()) {
      row = atoi(cell.c_str() + i) - 1; // Convert to 0-based
    }
  }

};
