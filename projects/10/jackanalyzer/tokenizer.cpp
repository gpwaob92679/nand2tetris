#include "tokenizer.h"

#include <fstream>
#include <iostream>
#include <limits>
#include <string>
#include <string_view>
#include <vector>

#include "absl/log/check.h"
#include "absl/log/log.h"
#include "absl/strings/ascii.h"
#include "absl/strings/charset.h"
#include "absl/strings/str_format.h"
#include "absl/strings/str_replace.h"

namespace nand2tetris {

namespace {

constexpr std::string_view kKeywords[] = {
    "class", "constructor", "function", "method", "field", "static", "var",
    "int",   "char",        "boolean",  "void",   "true",  "false",  "null",
    "this",  "let",         "do",       "if",     "else",  "while",  "return"};
bool IsKeyword(std::string_view token) {
  for (const std::string_view& keyword : kKeywords) {
    if (token == keyword) {
      return true;
    }
  }
  return false;
}

constexpr absl::CharSet kSymbols("{}()[].,;+-*/&|<>=~");

bool ReadComment(std::ifstream& file) {
  CHECK(file.get() == '/');
  if (file.peek() == '/') {  // Single-line comment.
    file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return true;
  } else if (file.peek() == '*') {  // Multi-line comment.
    file.get();
    char c;
    while (file.get(c)) {
      if (c == '*' && file.peek() == '/') {
        file.get();
        break;
      }
    }
    QCHECK(file) << "Unterminated multi-line comment";
    return true;
  }
  file.unget();
  return false;
}

Token ReadIntegerConstant(std::ifstream& file) {
  std::string value;
  while (absl::ascii_isdigit(file.peek())) {
    value.push_back(file.get());
  }
  QCHECK(!value.empty());
  return Token(TokenType::kIntegerConstant, std::move(value));
}

Token ReadStringConstant(std::ifstream& file) {
  CHECK(file.get() == '"');
  std::string value;
  char c;
  while (file.get(c)) {
    if (c == '"') {
      break;
    }
    value.push_back(c);
  }
  QCHECK(file) << "Unterminated string constant";
  return Token(TokenType::kStringConstant, std::move(value));
}

Token ReadKeywordOrIdentifier(std::ifstream& file) {
  std::string value;
  while (absl::ascii_isalnum(file.peek()) || file.peek() == '_') {
    value.push_back(file.get());
  }
  QCHECK(!value.empty());
  if (IsKeyword(value)) {
    return Token(TokenType::kKeyword, std::move(value));
  } else {
    return Token(TokenType::kIdentifier, std::move(value));
  }
}

}  // namespace

std::ostream& operator<<(std::ostream& os, TokenType type) {
  switch (type) {
    case TokenType::kKeyword:
      return os << "keyword";
    case TokenType::kSymbol:
      return os << "symbol";
    case TokenType::kIdentifier:
      return os << "identifier";
    case TokenType::kIntegerConstant:
      return os << "integerConstant";
    case TokenType::kStringConstant:
      return os << "stringConstant";
  }
  return os << "unknown";
}

std::vector<Token> Tokenize(std::string_view jack_file_path) {
  std::ifstream file(jack_file_path.data());
  QCHECK(file.is_open()) << "Failed to open file: " << jack_file_path;
  std::vector<Token> tokens;
  while (file) {
    // Ignore preceding spaces in stream.
    while (absl::ascii_isspace(file.peek())) {
      file.get();
    }
    if (file.peek() == std::char_traits<char>::eof()) {
      break;
    }

    if (file.peek() == '/' && ReadComment(file)) {
      continue;
    }

    if (absl::ascii_isdigit(file.peek())) {
      tokens.push_back(ReadIntegerConstant(file));
    } else if (file.peek() == '"') {
      tokens.push_back(ReadStringConstant(file));
    } else if (kSymbols.contains(file.peek())) {
      tokens.emplace_back(TokenType::kSymbol, std::string(1, file.get()));
    } else if (absl::ascii_isalpha(file.peek()) || file.peek() == '_') {
      tokens.push_back(ReadKeywordOrIdentifier(file));
    } else {
      LOG(FATAL) << "Invalid character: " << static_cast<char>(file.peek());
    }
  }
  file.close();
  return tokens;
}

}  // namespace nand2tetris
