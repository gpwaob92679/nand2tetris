#ifndef NAND2TETRIS_JACKANALYZER_PARSER_H_
#define NAND2TETRIS_JACKANALYZER_PARSER_H_

#include <iostream>
#include <string>
#include <variant>
#include <vector>

namespace nand2tetris {

struct Token;

enum class NodeType {
  kClass = 0,
  kClassVarDec,
  kSubroutineDec,
  kParameterList,
  kSubroutineBody,
  kVarDec,
  kStatements,
  kLetStatement,
  kIfStatement,
  kWhileStatement,
  kDoStatement,
  kReturnStatement,
  kExpression,
  kTerm,
  kExpressionList,
};

std::ostream& operator<<(std::ostream& os, NodeType type);

struct Node {
  NodeType type;
  std::vector<std::variant<Node, Token>> children;

  Node(NodeType type) : type(type) {}
};

Node Parse(const std::vector<Token>& tokens);

}  // namespace nand2tetris

#endif  // NAND2TETRIS_JACKANALYZER_PARSER_H_
