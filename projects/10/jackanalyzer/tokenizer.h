#ifndef NAND2TETRIS_JACKANALYZER_TOKENIZER_H_
#define NAND2TETRIS_JACKANALYZER_TOKENIZER_H_

#include <fstream>
#include <string>
#include <string_view>

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

class JackFile {
 public:
  JackFile(std::string_view path);

  void Advance();

  Token* token() const { return token_; }

 private:
  bool ReadComment();
  void ReadIntegerConstant();
  void ReadStringConstant();
  void ReadKeywordOrIdentifier();

  std::ifstream file_;
  std::string path_;
  Token* token_ = nullptr;
};

}  // namespace nand2tetris

#endif  // NAND2TETRIS_JACKANALYZER_TOKENIZER_H_
