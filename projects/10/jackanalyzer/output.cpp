#include "output.h"

#include <string_view>
#include <variant>

#include "absl/log/check.h"
#include "absl/log/log.h"
#include "absl/strings/str_format.h"
#include "absl/strings/str_replace.h"

#include "parser.h"
#include "tokenizer.h"

namespace nand2tetris {

namespace {
inline std::string Escape(std::string_view str) {
  return absl::StrReplaceAll(str, {{"<", "&lt;"},
                                   {">", "&gt;"},
                                   {"&", "&amp;"},
                                   {"\"", "&quot;"},
                                   {"'", "&apos;"}});
}
}  // namespace

XmlFile::XmlFile(std::string_view path, bool verbose)
    : file_(path.data()), verbose_(verbose) {
  QCHECK(file_.is_open()) << "Could not open output file: " << path;
  LOG(INFO) << "Output: " << path;
}

XmlFile::~XmlFile() { file_.close(); }

XmlFile& XmlFile::operator<<(const Token& token) {
  std::string type_str;
  switch (token.type) {
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
  std::string xml_element =
      absl::StrFormat("<%s> %s </%s>", type_str, Escape(token.value), type_str);
  if (verbose_) {
    LOG(INFO) << xml_element;
  }
  WriteIndentation();
  file_ << xml_element << std::endl;
  return *this;
}

void XmlFile::operator<<(const std::vector<Token>& tokens) {
  WriteIndentation();
  file_ << "<tokens>" << std::endl;

  Indent();
  for (const Token& token : tokens) {
    *this << token;
  }
  Unindent();

  WriteIndentation();
  file_ << "</tokens>" << std::endl;
}

XmlFile& XmlFile::operator<<(const Node& node) {
  WriteIndentation();
  file_ << '<' << node.type << '>' << std::endl;

  // Recursively write children to file.
  Indent();
  for (const std::variant<Node, Token>& child : node.children) {
    std::visit([this](auto&& arg) { *this << arg; }, child);
  }
  Unindent();

  WriteIndentation();
  file_ << "</" << node.type << '>' << std::endl;
  return *this;
}

}  // namespace nand2tetris
