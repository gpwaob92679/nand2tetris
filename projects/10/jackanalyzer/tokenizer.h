#ifndef NAND2TETRIS_JACKANALYZER_TOKENIZER_H_
#define NAND2TETRIS_JACKANALYZER_TOKENIZER_H_

#include <memory>
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

class Token {
 public:
  Token(TokenType type, std::string_view value);

  std::string ToXmlElement() const;

  static bool IsKeyword(std::string_view token) {
    for (const std::string_view& keyword : kKeywords) {
      if (token == keyword) {
        return true;
      }
    }
    return false;
  }

  static bool IsSymbol(char c) {
    return kSymbols.find(c) != std::string_view::npos;
  }

 private:
  static constexpr std::string_view kKeywords[] = {
      "class", "constructor", "function", "method",  "field", "static",
      "var",   "int",         "char",     "boolean", "void",  "true",
      "false", "null",        "this",     "let",     "do",    "if",
      "else",  "while",       "return"};
  static constexpr std::string_view kSymbols = "{}()[].,;+-*/&|<>=~";

  TokenType type_;
  std::string value_;
};

std::vector<std::unique_ptr<Token>> Tokenize(std::string_view jack_file_path);

}  // namespace nand2tetris

#endif  // NAND2TETRIS_JACKANALYZER_TOKENIZER_H_
