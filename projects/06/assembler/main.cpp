#include <filesystem>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "absl/log/check.h"
#include "absl/log/globals.h"
#include "absl/log/initialize.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"

#include "instruction.h"

inline bool IsNumber(std::string_view str) {
  for (char c : str) {
    if (!isdigit(c)) {
      return false;
    }
  }
  return true;
}

inline bool IsLabel(std::string_view str) {
  return str.front() == '(' && str.back() == ')';
}

int main(int argc, char* argv[]) {
  absl::InitializeLog();
  absl::SetStderrThreshold(absl::LogSeverity::kInfo);

  QCHECK_GE(argc, 2) << "Usage: " << argv[0] << " SOURCE";
  std::ifstream asm_file(argv[1]);
  QCHECK(asm_file.is_open()) << "Failed to open input file " << argv[1];
  LOG(INFO) << "Source: " << argv[1];

  // Trim comments and whitespaces.
  std::string buffer;
  std::vector<std::string> trimmed_lines;
  while (std::getline(asm_file, buffer)) {
    std::string line;
    for (size_t i = 0; i < buffer.size(); ++i) {
      if (buffer[i] == ' ' || buffer[i] == '\t') {
        continue;
      }
      if (buffer.substr(i, 2) == "//") {
        break;
      }
      line.push_back(buffer[i]);
    }
    if (!line.empty()) {
      trimmed_lines.push_back(std::move(line));
    }
  }
  asm_file.close();

  // First pass: map labels to instruction addresses.
  std::unordered_map<std::string, uint16_t> symbol_table = {
      {"R0", 0},   {"R1", 1},         {"R2", 2},      {"R3", 3},   {"R4", 4},
      {"R5", 5},   {"R6", 6},         {"R7", 7},      {"R8", 8},   {"R9", 9},
      {"R10", 10}, {"R11", 11},       {"R12", 12},    {"R13", 13}, {"R14", 14},
      {"R15", 15}, {"SCREEN", 16384}, {"KBD", 24576}, {"SP", 0},   {"LCL", 1},
      {"ARG", 2},  {"THIS", 3},       {"THAT", 4}};
  uint16_t instruction_counter = 0;
  for (const std::string& line : trimmed_lines) {
    if (IsLabel(line)) {  // Is label.
      symbol_table[line.substr(1, line.size() - 2)] = instruction_counter;
    } else {
      ++instruction_counter;
    }
  }

  std::filesystem::path hack_path(argv[1]);
  hack_path.replace_extension(".hack");
  std::ofstream hack_file(hack_path);
  QCHECK(hack_file.is_open())
      << "Failed to open output file " << hack_path.string();
  LOG(INFO) << "Output: " << hack_path.string();

  // Second pass: translate instructions.
  uint16_t variable_address = 16;
  for (const std::string& line : trimmed_lines) {
    if (IsLabel(line)) {
      continue;
    }
    if (line.front() == '@') {  // A-instruction
      std::string value_str = line.substr(1);
      if (IsNumber(value_str)) {
        uint32_t value;
        if (absl::SimpleAtoi(value_str, &value) && value <= UINT16_MAX) {
          hack_file << nand2tetris::AInstruction(static_cast<uint16_t>(value))
                           .ToMachine()
                    << std::endl;
        } else {
          LOG(ERROR) << "Cannot convert " << value_str << " to uint16_t";
        }
      } else {
        if (!symbol_table.count(value_str)) {  // New variable.
          symbol_table[value_str] = variable_address++;
        }
        hack_file
            << nand2tetris::AInstruction(symbol_table[value_str]).ToMachine()
            << std::endl;
      }
    } else {  // C-instruction
      nand2tetris::CInstruction instruction;
      std::string_view line_view(line);
      size_t found = line_view.find('=');
      if (found != std::string_view::npos) {
        instruction.SetDestination(line_view.substr(0, found));
        line_view.remove_prefix(found + 1);
      }

      found = line_view.find(';');
      instruction.SetComputation(line_view.substr(0, found));
      if (found != std::string_view::npos) {
        line_view.remove_prefix(found + 1);
        instruction.SetJump(line_view);
      }
      hack_file << instruction.ToMachine() << std::endl;
    }
  }
  hack_file.close();
  return 0;
}
