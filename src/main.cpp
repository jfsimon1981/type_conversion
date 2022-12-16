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
      // Raw integer
      // uint32_t vi = std::stoi(str_read); // If string is a raw integer
      // If string is an hexadecimal formatted 0x... ex. 0x10, 0x1f, 0xffff, ...

      // Hex -> uint32_t conversion
      uint32_t vi {0};
      {
        // Del 0x
        str_read.erase(0,2);
        // Std conversion
        unsigned int hex_val;
        std::stringstream hex_val_str;
        hex_val_str << std::hex << str_read; // Ex. str_read == "01d0ffac";
        hex_val_str >> hex_val;
        // Store external
        vi = hex_val;
      }

      table.push_back(vi);
      #if READBACK_FILE == 1
      cout << "Read line[" << line++ << "] == " << str_read << " (" << vi << ")" << endl;
      #endif
    }
    cout << "File " << data_file_in << " read. Number of lines: " << table.size() << endl;
  }

  // Read intervals
  #if SHOW_DELTAS == 1
  {
    cout << "Read deltas: " << endl;
    uint32_t i_prec {0x00};
    uint32_t line {0};
    for (auto i:table) {
      auto dx {i - i_prec};
      cout << "di[" << line << "] == " << table_raw_str_in[line] \
        << " Size of in uint_32 == " << dx/4 << ". " \
        << " Size of in bytes == " << dx << endl;
      // Store deltas
      data_file_ofs << table_raw_str_in[line] << " " << dx/4 << endl;
      // Store formated
      {
        if (dx/4 == 1) {
          string line_str {"VAR"};
          line_str += to_string(line);
          data_file_out_formated_ofs << line_str << endl;
        } else {
          string line_str {"VAR"};
          line_str += to_string(line);
          line_str += "[";
          line_str += to_string(dx/4);
          line_str += "]";
          data_file_out_formated_ofs << line_str << endl;
        }
      }
      // Reinit
      i_prec = i;
      line++;
    }
    cout << endl;
  }
  #endif
}
