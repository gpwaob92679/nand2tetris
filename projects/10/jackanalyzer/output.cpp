#include "output.h"

#include <string_view>

#include "absl/log/check.h"
#include "absl/log/log.h"

#include "tokenizer.h"

namespace nand2tetris {

TokensXmlFile::TokensXmlFile(std::string_view path)
    : file_(path.data()), path_(path) {
  QCHECK(file_.is_open()) << "Could not open output file: " << path;
  LOG(INFO) << "Output: " << path;

  file_ << "<tokens>" << std::endl;
}

TokensXmlFile::~TokensXmlFile() { file_ << "</tokens>" << std::endl; }

TokensXmlFile& TokensXmlFile::operator<<(const Token& token) {
  file_ << "  " << token.ToXmlElement() << std::endl;
  return *this;
}

std::string TokensXmlFile::path() const { return path_; }

}  // namespace nand2tetris
