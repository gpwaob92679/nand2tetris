#ifndef NAND2TETRIS_JACKANALYZER_OUTPUT_H_
#define NAND2TETRIS_JACKANALYZER_OUTPUT_H_

#include <fstream>
#include <string_view>

#include "tokenizer.h"

namespace nand2tetris {

class TokensXmlFile {
 public:
  TokensXmlFile(std::string_view path);
  ~TokensXmlFile();

  std::string path() const;

  TokensXmlFile& operator<<(const Token& x);

 private:
  std::ofstream file_;
  std::string path_;
};

}  // namespace nand2tetris

#endif  // NAND2TETRIS_JACKANALYZER_OUTPUT_H_
