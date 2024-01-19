#include "tokenizer.h"

#include <cctype>
#include <limits>
#include <string>
#include <string_view>

#include "absl/log/check.h"
#include "absl/log/log.h"
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

  TokenType type;
  std::string value;
  for (char c; file_.get(c);) {
    // Ignore preceding spaces in stream.
    while (isspace(c)) {
      if (!file_.get(c)) {
        return;
      }
    }

    // Comment.
    if (c == '/') {
      if (file_.peek() == '/') {  // Single-line comment.
        file_.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        continue;
      }
      if (file_.peek() == '*') {  // Multi-line comment.
        file_.get();
        while (file_.get(c)) {
          if (c == '*' && file_.peek() == '/') {
            file_.get();
            break;
          }
        }
        continue;
      }
    }

    // Integer constant.
    if (isdigit(c)) {
      value.push_back(c);
      while (isdigit(file_.peek())) {
        value.push_back(file_.get());
      }
      type = TokenType::kIntegerConstant;
      break;
    }

    // String constant.
    if (c == '"') {
      while (file_.get(c)) {
        if (c == '"') {
          break;
        }
        value.push_back(c);
      }
      if (!file_) {
        LOG(ERROR) << "Unterminated string constant: ";
      }
      type = TokenType::kStringConstant;
      break;
    }

    // Symbol.
    if (IsSymbol(c)) {
      value.push_back(c);
      type = TokenType::kSymbol;
      break;
    }

    // Keyword or identifier.
    if (isalpha(c) || c == '_') {
      value.push_back(c);
      while (isalnum(file_.peek()) || isdigit(file_.peek()) ||
             file_.peek() == '_') {
        value.push_back(file_.get());
      }

      if (IsKeyword(value)) {
        type = TokenType::kKeyword;
        break;
      }
      type = TokenType::kIdentifier;
      break;
    }

    LOG(ERROR) << "Unkown token: '" << value << "'";
  }
  token_ = new Token(type, value);
}

std::string JackFile::path() const { return path_; }
Token* JackFile::token() { return token_; }

bool JackFile::IsKeyword(std::string_view token) {
  for (const std::string_view& keyword : kKeywords) {
    if (token == keyword) {
      return true;
    }
  }
  return false;
}

bool JackFile::IsSymbol(char c) {
  return kSymbols.find(c) != std::string_view::npos;
}

}  // namespace nand2tetris
