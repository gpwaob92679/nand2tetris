#include "output.h"

#include <string_view>

#include "absl/log/check.h"
#include "absl/log/log.h"

#include "commands.h"

namespace nand2tetris {

AssemblyFile::AssemblyFile(std::string_view path, bool source_is_multi_file)
    : file_(path.data()), source_is_multi_file_(source_is_multi_file) {
  QCHECK(file_.is_open()) << "Could not open output file: " << path;
  LOG(INFO) << "Output: " << path;

  // Bootstrap code.
  file_ << "@256\n"
        << "D=A\n"
        << "@SP\n"
        << "M=D\n";

  if (source_is_multi_file_) {
    file_ << CallCommand("Sys.init", 0, "END").ToAssembly();
    // Although `Sys.init` is expected to enter an infinite loop, we still add
    // an infinite loop in case `Sys.init` returns.
    file_ << "@END\n"
          << "(END)\n"
          << "0;JMP\n";
  }
}

AssemblyFile::~AssemblyFile() {
  if (!source_is_multi_file_) {
    file_ << "@END\n"
          << "(END)\n"
          << "0;JMP\n";
  }
  file_.close();
}

}  // namespace nand2tetris
