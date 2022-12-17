#define READBACK_FILE 0 // Print to screen file lines while file is read into vector<string>.
#define SHOW_DELTAS 1   // Prints values delta line by line

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;
int main() {
  vector<uint32_t> table;
  vector<string> table_raw_str_in; // Raw data
  // Filtered data
  vector<string> table_field_1_raw_str_in; // Field 1: Register address offset
  vector<string> table_field_2_raw_str_in; // Field 2: Register name
  vector<string> table_field_3_raw_str_in; // Field 3: Register description

  // Test sample
  /*
  table.push_back(0x00);table_raw_str_in.push_back("0x00"); // Ref.
  table.push_back(0x04);table_raw_str_in.push_back("0x04"); // Delta +4 (1 32b Word)
  table.push_back(0x08);table_raw_str_in.push_back("0x08"); // Delta +4 (1 32b Word)
  table.push_back(0x10);table_raw_str_in.push_back("0x10"); // Delta +8 (2 32b Word)
  table.push_back(0xff);table_raw_str_in.push_back("0xff"); // Delta +239 (59 32b Word)
  */

  // Read data from file
  const string data_file_in {"data_in.txt"};   // Raw address in
  const string data_file_out {"data_out.txt"}; // Delta calculations
  const string data_file_out_formated {"data_out_formated.txt"}; // [] format when needed
  ifstream data_file_ifs {data_file_in};
  ofstream data_file_ofs {data_file_out};
  ofstream data_file_out_formated_ofs {data_file_out_formated};
  {
    string str_read;
    unsigned int line {0};
    while (getline(data_file_ifs, str_read)) {
      table_raw_str_in.push_back(str_read);
      stringstream line_strm {str_read};
      string field_1, field_2, field_3;
      line_strm >> field_1 >> field_2 >> field_3;

      table_field_1_raw_str_in.push_back(field_1);
      table_field_2_raw_str_in.push_back(field_2);
      table_field_3_raw_str_in.push_back(field_3);

      // Raw integer
      // uint32_t vi = std::stoi(field_1); // If string is a raw integer
      // If string is an hexadecimal formatted 0x... ex. 0x10, 0x1f, 0xffff, ...

      // Hex -> uint32_t conversion
      uint32_t vi {0};
      {
        // Del 0x
        field_1.erase(0,2);
        // Std conversion
        unsigned int hex_val;
        std::stringstream hex_val_str;
        hex_val_str << std::hex << field_1; // Ex. field_1 == "01d0ffac";
        hex_val_str >> hex_val;
        // Store external
        vi = hex_val;
      }

      table.push_back(vi);

      #if READBACK_FILE == 1
      cout << "Read line[" << line++ << "] == " << field_1 << " (" << vi << ")" << endl;
      #endif
    }
    cout << "File " << data_file_in << " read. Number of lines: " << table.size() << endl;
  }

  // Read intervals
  #if SHOW_DELTAS == 1
  {
    cout << "Read deltas: " << endl;
    uint32_t line {0};
    uint32_t var_count {0}; // For unnamed variables
    for (auto i:table) {
      if ((line + 1) == table.size()) break; // Skip last element (end element)
      auto dx {table[line+1] - table[line]}; // *(i+1) - i
      cout << "di[" << line << "] == " << table_raw_str_in[line] \
        << " Size of in uint_32 == " << dx/4 << ". " \
        << " Size of in bytes == " << dx << endl;
      // Store deltas
      data_file_ofs << table_raw_str_in[line] << " " << dx/4 << endl;
      // Store formated
      // Create formatted string style
      //  volatile uint32_t EIR; // 0x4
      //  volatile uint32_t EIMR; // 0x08
      //  volatile uint32_t RDAR[2]; // 0x10
      {
        string line_str;
        // Prepend
        line_str += "volatile uint32_t ";
        // Conditional formating (single element or, arrays, named or anonymous)
        if (dx/4 == 1) {                                // Single element
          if (table_field_2_raw_str_in[line].size() > 0)
            line_str += table_field_2_raw_str_in[line]; // Named element
          else {                                        // Anonymous element
            line_str += "RESERVED";
            line_str += to_string(var_count);
            var_count++;
          }
          line_str += "; // ";
          line_str += table_field_1_raw_str_in[line];

          if (table_field_3_raw_str_in[line].size() > 0) { // Add description
            line_str += " ";
            line_str += table_field_3_raw_str_in[line];
          }

          data_file_out_formated_ofs << line_str << endl;
        } else {                                        // Element array[]
          if (table_field_2_raw_str_in[line].size() > 0)
            line_str += table_field_2_raw_str_in[line]; // Named array
          else {                                        // Anonymous array
            line_str += "RESERVED";
            line_str += to_string(var_count);
            var_count++;
          }
          line_str += "[";
          line_str += to_string(dx/4);
          line_str += "]";
          line_str += "; // ";
          line_str += table_field_1_raw_str_in[line];

          if (table_field_3_raw_str_in[line].size() > 0) { // Add description
            line_str += " ";
            line_str += table_field_3_raw_str_in[line];
          }

          data_file_out_formated_ofs << line_str << endl;
        }
      }
      // Reinit
      line++;
    }
    cout << endl;
  }
  // Clang format
  system("./f");
  #endif
}
