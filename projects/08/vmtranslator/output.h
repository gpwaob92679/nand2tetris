#ifndef NAND2TETRIS_VMTRANSLATOR_OUTPUT_H_
#define NAND2TETRIS_VMTRANSLATOR_OUTPUT_H_

#include <fstream>
#include <string_view>

class AssemblyFile {
 public:
  AssemblyFile(std::string_view path, bool source_is_multi_file);
  ~AssemblyFile();

  template <class T>
  AssemblyFile& operator<<(const T& value) {
    file_ << value;
    return *this;
  }

 private:
  std::ofstream file_;
  bool source_is_multi_file_ = false;
};

#endif  // NAND2TETRIS_VMTRANSLATOR_OUTPUT_H_
