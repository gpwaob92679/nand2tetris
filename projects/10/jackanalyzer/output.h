#ifndef NAND2TETRIS_JACKANALYZER_OUTPUT_H_
#define NAND2TETRIS_JACKANALYZER_OUTPUT_H_

#include <fstream>
#include <string_view>
#include <vector>

namespace nand2tetris {

struct Node;
struct Token;

class XmlFile {
 public:
  XmlFile(std::string_view path, bool verbose);
  ~XmlFile();

  XmlFile& operator<<(const Token& token);
  void operator<<(const std::vector<Token>& tokens);
  XmlFile& operator<<(const Node& node);

 private:
  void Indent() { indentation_ += 2; }
  void Unindent() { indentation_ -= 2; }
  void WriteIndentation() {
    for (int i = 0; i < indentation_; ++i) {
      file_ << ' ';
    }
  }

  std::ofstream file_;
  bool verbose_;
  int indentation_ = 0;
};

}  // namespace nand2tetris

#endif  // NAND2TETRIS_JACKANALYZER_OUTPUT_H_
