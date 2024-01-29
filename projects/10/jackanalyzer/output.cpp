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
  std::string xml_element =
      absl::StrFormat("<%s> %s </%s>", absl::FormatStreamed(token.type),
                      Escape(token.value), absl::FormatStreamed(token.type));
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
