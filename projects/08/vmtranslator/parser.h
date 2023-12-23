
#ifndef NAND2TETRIS_VMTRANSLATOR_PARSER_H_
#define NAND2TETRIS_VMTRANSLATOR_PARSER_H_

#include <fstream>
#include <string_view>

#include "commands.h"

class VmFile {
 public:
  VmFile(std::string_view filename);
  ~VmFile();

  void VmFile::Advance();

  std::string line();
  Command *command();

 private:
  std::unique_ptr<Address> ParseAddress(std::string_view segment,
                                        std::string_view index_str);

  std::string filename_;
  std::ifstream file_;

  std::string line_;
  size_t line_number_ = 0;
  Command *command_ = nullptr;
};

#endif  // NAND2TETRIS_VMTRANSLATOR_PARSER_H_
