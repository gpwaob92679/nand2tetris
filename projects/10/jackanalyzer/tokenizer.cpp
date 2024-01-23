#include "tokenizer.h"

#include <fstream>
#include <limits>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "absl/log/check.h"
#include "absl/log/log.h"
#include "absl/strings/ascii.h"
#include "absl/strings/str_format.h"
#include "absl/strings/str_replace.h"

namespace nand2tetris {

Token::Token(TokenType type, std::string_view value)
    : type_(type), value_(value) {}

std::string Token::ToXmlElement() const {
  std::string type_str;
  switch (type_) {
    case TokenType::kKeyword:
      type_str = "keyword";
      break;
    case TokenType::kSymbol:
      type_str = "symbol";
      break;
    case TokenType::kIdentifier:
      type_str = "identifier";
      break;
    case TokenType::kIntegerConstant:
      type_str = "integerConstant";
      break;
    case TokenType::kStringConstant:
      type_str = "stringConstant";
      break;
  }
  return absl::StrFormat("<%s> %s </%s>", type_str,
                         absl::StrReplaceAll(value_, {{"<", "&lt;"},
                                                      {">", "&gt;"},
                                                      {"&", "&amp;"},
                                                      {"\"", "&quot;"},
                                                      {"'", "&apos;"}}),
                         type_str);
}

namespace {

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

std::unique_ptr<Token> ReadIntegerConstant(std::ifstream& file) {
  std::string value;
  while (absl::ascii_isdigit(file.peek())) {
    value.push_back(file.get());
  }
  QCHECK(!value.empty());
  return std::make_unique<Token>(TokenType::kIntegerConstant, std::move(value));
}

std::unique_ptr<Token> ReadStringConstant(std::ifstream& file) {
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
  return std::make_unique<Token>(TokenType::kStringConstant, std::move(value));
}

std::unique_ptr<Token> ReadKeywordOrIdentifier(std::ifstream& file) {
  std::string value;
  while (absl::ascii_isalnum(file.peek()) || file.peek() == '_') {
    value.push_back(file.get());
  }
  QCHECK(!value.empty());
  if (Token::IsKeyword(value)) {
    return std::make_unique<Token>(TokenType::kKeyword, std::move(value));
  } else {
    return std::make_unique<Token>(TokenType::kIdentifier, std::move(value));
  }
}

}  // namespace

std::vector<std::unique_ptr<Token>> Tokenize(std::string_view jack_file_path) {
  std::ifstream file(jack_file_path.data());
  QCHECK(file.is_open()) << "Failed to open file: " << jack_file_path;
  std::vector<std::unique_ptr<Token>> tokens;
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
    } else if (Token::IsSymbol(file.peek())) {
      tokens.push_back(std::make_unique<Token>(TokenType::kSymbol,
                                               std::string(1, file.get())));
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
