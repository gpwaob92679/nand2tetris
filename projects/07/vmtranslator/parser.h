
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
  Command *command();

 private:
  std::string filename_;
  std::ifstream file_;
  size_t line_number_ = 0;
  Command *command_ = nullptr;
};

#endif  // NAND2TETRIS_VMTRANSLATOR_PARSER_H_
