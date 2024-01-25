#ifndef NAND2TETRIS_JACKANALYZER_TOKENIZER_H_
#define NAND2TETRIS_JACKANALYZER_TOKENIZER_H_

#include <string>
#include <string_view>
#include <vector>

namespace nand2tetris {

enum class TokenType {
  kKeyword = 0,
  kSymbol,
  kIdentifier,
  kIntegerConstant,
  kStringConstant,
};

struct Token {
  TokenType type;
  std::string value;

  Token(TokenType type, std::string_view value) : type(type), value(value) {}
};

std::vector<Token> Tokenize(std::string_view jack_file_path);

}  // namespace nand2tetris

#endif  // NAND2TETRIS_JACKANALYZER_TOKENIZER_H_
