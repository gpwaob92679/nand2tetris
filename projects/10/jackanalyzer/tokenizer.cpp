#include "tokenizer.h"

#include <limits>
#include <string>
#include <string_view>

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

JackFile::JackFile(std::string_view path) : file_(path.data()), path_(path) {
  QCHECK(file_.is_open()) << "Failed to open file: " << path;
  Advance();
}

void JackFile::Advance() {
  if (token_) {
    delete token_;
    token_ = nullptr;
  }

  while (file_) {
    // Ignore preceding spaces in stream.
    while (absl::ascii_isspace(file_.peek())) {
      file_.get();
    }
    if (file_.peek() == std::char_traits<char>::eof()) {
      file_.get();
      return;
    }

    if (file_.peek() == '/' && ReadComment()) {
      continue;
    }

    if (absl::ascii_isdigit(file_.peek())) {
      ReadIntegerConstant();
      return;
    }
    if (file_.peek() == '"') {
      ReadStringConstant();
      return;
    }
    if (Token::IsSymbol(file_.peek())) {
      token_ = new Token(TokenType::kSymbol, std::string(1, file_.get()));
      return;
    }
    if (absl::ascii_isalpha(file_.peek()) || file_.peek() == '_') {
      ReadKeywordOrIdentifier();
      return;
    }
    LOG(FATAL) << "Invalid character: " << static_cast<char>(file_.peek());
  }
}

bool JackFile::ReadComment() {
  CHECK(file_.get() == '/');
  if (file_.peek() == '/') {  // Single-line comment.
    file_.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return true;
  } else if (file_.peek() == '*') {  // Multi-line comment.
    file_.get();
    char c;
    while (file_.get(c)) {
      if (c == '*' && file_.peek() == '/') {
        file_.get();
        break;
      }
    }
    QCHECK(file_) << "Unterminated multi-line comment";
    return true;
  }
  file_.unget();
  return false;
}

void JackFile::ReadIntegerConstant() {
  std::string value;
  while (absl::ascii_isdigit(file_.peek())) {
    value.push_back(file_.get());
  }
  QCHECK(!value.empty());
  token_ = new Token(TokenType::kIntegerConstant, std::move(value));
}

void JackFile::ReadStringConstant() {
  CHECK(file_.get() == '"');
  std::string value;
  char c;
  while (file_.get(c)) {
    if (c == '"') {
      break;
    }
    value.push_back(c);
  }
  QCHECK(file_) << "Unterminated string constant";
  token_ = new Token(TokenType::kStringConstant, std::move(value));
}

void JackFile::ReadKeywordOrIdentifier() {
  std::string value;
  while (absl::ascii_isalnum(file_.peek()) || file_.peek() == '_') {
    value.push_back(file_.get());
  }
  QCHECK(!value.empty());
  if (Token::IsKeyword(value)) {
    token_ = new Token(TokenType::kKeyword, std::move(value));
  } else {
    token_ = new Token(TokenType::kIdentifier, std::move(value));
  }
}

}  // namespace nand2tetris
