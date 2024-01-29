#include "parser.h"

#include <iostream>
#include <vector>

#include "absl/log/check.h"
#include "absl/log/log.h"
#include "absl/strings/charset.h"

#include "tokenizer.h"

namespace nand2tetris {

namespace {

// Internal helper class.
class Parser {
 public:
  using TokenIterator = std::vector<Token>::const_iterator;

  Parser(const TokenIterator& it, const TokenIterator& end)
      : it_(it), end_(end) {}

  // Should be called only once.
  Node ParseClass() {
    Node node(NodeType::kClass);
    ConsumeToken(node, it_->value == "class");
    ConsumeToken(node, it_->type == TokenType::kIdentifier);  // className
    ConsumeToken(node, it_->value == "{");

    // classVarDec*
    while (it_ != end_ && (it_->value == "static" || it_->value == "field")) {
      node.children.push_back(ParseClassVarDec());
    }
    // subroutineDec*
    while (it_ != end_ &&
           (it_->value == "constructor" || it_->value == "function" ||
            it_->value == "method")) {
      node.children.push_back(ParseSubroutineDec());
    }

    ConsumeToken(node, it_->value == "}");
    return node;
  }

 private:
  static constexpr absl::CharSet kOps = absl::CharSet("+-*/&|<>=");
  static constexpr absl::CharSet kUnaryOps = absl::CharSet("-~");

  inline void ConsumeToken(Node& node, bool constraint) {
    CHECK(it_ != end_) << "Unexpected end of input";
    CHECK(constraint);
    node.children.push_back(*it_);
    ++it_;
  }

  // Current token matches `type` non-terminal in the Jack language
  // specification.
  inline bool IsType() {
    return it_->value == "int" || it_->value == "char" ||
           it_->value == "boolean" ||
           it_->type == TokenType::kIdentifier;  // className
  }

  Node ParseClassVarDec() {
    Node node(NodeType::kClassVarDec);
    ConsumeToken(node, it_->value == "static" || it_->value == "field");
    ConsumeToken(node, IsType());
    ConsumeToken(node, it_->type == TokenType::kIdentifier);  // varName

    // (',' varName)*
    while (it_ != end_ && it_->value == ",") {
      ConsumeToken(node, it_->value == ",");
      ConsumeToken(node, it_->type == TokenType::kIdentifier);  // varName
    }

    ConsumeToken(node, it_->value == ";");
    return node;
  }

  Node ParseSubroutineDec() {
    Node node(NodeType::kSubroutineDec);
    ConsumeToken(node, it_->value == "constructor" ||
                           it_->value == "function" || it_->value == "method");
    ConsumeToken(node, it_->value == "void" || IsType());
    ConsumeToken(node, it_->type == TokenType::kIdentifier);  // subroutineName
    ConsumeToken(node, it_->value == "(");
    node.children.push_back(ParseParameterList());
    ConsumeToken(node, it_->value == ")");
    node.children.push_back(ParseSubroutineBody());
    return node;
  }

  Node ParseParameterList() {
    Node node(NodeType::kParameterList);
    // May contain no children (`<parameterList> </parameterList>`).
    if (it_ != end_ && IsType()) {
      ConsumeToken(node, IsType());
      ConsumeToken(node, it_->type == TokenType::kIdentifier);  // varName

      // (',' type varName)*
      while (it_ != end_ && it_->value == ",") {
        ConsumeToken(node, it_->value == ",");
        ConsumeToken(node, IsType());
        ConsumeToken(node, it_->type == TokenType::kIdentifier);  // varName
      }
    }
    return node;
  }

  Node ParseSubroutineBody() {
    Node node(NodeType::kSubroutineBody);
    ConsumeToken(node, it_->value == "{");
    while (it_ != end_ && it_->value == "var") {
      node.children.push_back(ParseVarDec());
    }
    node.children.push_back(ParseStatements());
    ConsumeToken(node, it_->value == "}");
    return node;
  }

  Node ParseVarDec() {
    Node node(NodeType::kVarDec);
    ConsumeToken(node, it_->value == "var");
    ConsumeToken(node, IsType());
    ConsumeToken(node, it_->type == TokenType::kIdentifier);  // varName

    // (',' varName)*
    while (it_ != end_ && it_->value == ",") {
      ConsumeToken(node, it_->value == ",");
      ConsumeToken(node, it_->type == TokenType::kIdentifier);  // varName
    }
    ConsumeToken(node, it_->value == ";");
    return node;
  }

  Node ParseStatements() {
    Node node(NodeType::kStatements);
    while (it_ != end_) {
      if (it_->value == "let") {
        node.children.push_back(ParseLetStatement());
      } else if (it_->value == "if") {
        node.children.push_back(ParseIfStatement());
      } else if (it_->value == "while") {
        node.children.push_back(ParseWhileStatement());
      } else if (it_->value == "do") {
        node.children.push_back(ParseDoStatement());
      } else if (it_->value == "return") {
        node.children.push_back(ParseReturnStatement());
      } else {
        break;
      }
    }
    return node;
  }

  Node ParseLetStatement() {
    Node node(NodeType::kLetStatement);
    ConsumeToken(node, it_->value == "let");
    ConsumeToken(node, it_->type == TokenType::kIdentifier);  // varName
    if (it_ != end_ && it_->value == "[") {
      ConsumeToken(node, it_->value == "[");
      node.children.push_back(ParseExpression());
      ConsumeToken(node, it_->value == "]");
    }
    ConsumeToken(node, it_->value == "=");
    node.children.push_back(ParseExpression());
    ConsumeToken(node, it_->value == ";");
    return node;
  }

  Node ParseIfStatement() {
    Node node(NodeType::kIfStatement);
    ConsumeToken(node, it_->value == "if");
    ConsumeToken(node, it_->value == "(");
    node.children.push_back(ParseExpression());
    ConsumeToken(node, it_->value == ")");
    ConsumeToken(node, it_->value == "{");
    node.children.push_back(ParseStatements());
    ConsumeToken(node, it_->value == "}");
    if (it_ != end_ && it_->value == "else") {
      ConsumeToken(node, it_->value == "else");
      ConsumeToken(node, it_->value == "{");
      node.children.push_back(ParseStatements());
      ConsumeToken(node, it_->value == "}");
    }
    return node;
  }

  Node ParseWhileStatement() {
    Node node(NodeType::kWhileStatement);
    ConsumeToken(node, it_->value == "while");
    ConsumeToken(node, it_->value == "(");
    node.children.push_back(ParseExpression());
    ConsumeToken(node, it_->value == ")");
    ConsumeToken(node, it_->value == "{");
    node.children.push_back(ParseStatements());
    ConsumeToken(node, it_->value == "}");
    return node;
  }

  Node ParseDoStatement() {
    Node node(NodeType::kDoStatement);
    ConsumeToken(node, it_->value == "do");

    // subroutineName | (className | varName)
    ConsumeToken(node, it_->type == TokenType::kIdentifier);
    if (it_ != end_ && it_->value == ".") {
      ConsumeToken(node, it_->value == ".");
      ConsumeToken(node, it_->type == TokenType::kIdentifier);
    }

    ConsumeToken(node, it_->value == "(");
    node.children.push_back(ParseExpressionList());
    ConsumeToken(node, it_->value == ")");
    ConsumeToken(node, it_->value == ";");
    return node;
  }

  Node ParseReturnStatement() {
    Node node(NodeType::kReturnStatement);
    ConsumeToken(node, it_->value == "return");
    if (it_ != end_ && it_->value != ";") {
      node.children.push_back(ParseExpression());
    }
    ConsumeToken(node, it_->value == ";");
    return node;
  }

  Node ParseExpression() {
    Node node(NodeType::kExpression);
    node.children.push_back(ParseTerm());

    // (op term)*
    while (it_ != end_ && it_->type == TokenType::kSymbol &&
           kOps.contains(it_->value[0])) {
      ConsumeToken(node, it_->type == TokenType::kSymbol &&
                             kOps.contains(it_->value[0]));
      node.children.push_back(ParseTerm());
    }
    return node;
  }

  Node ParseTerm() {
    Node node(NodeType::kTerm);
    if (it_ == end_) {
      return node;
    }
    if (it_->type == TokenType::kSymbol && kUnaryOps.contains(it_->value[0])) {
      ConsumeToken(node, it_->type == TokenType::kSymbol &&
                             kUnaryOps.contains(it_->value[0]));
      node.children.push_back(ParseTerm());
      return node;
    }
    if (it_->value == "(") {
      ConsumeToken(node, it_->value == "(");
      node.children.push_back(ParseExpression());
      ConsumeToken(node, it_->value == ")");
      return node;
    }
    if (it_->type == TokenType::kIdentifier) {
      ConsumeToken(node, it_->type == TokenType::kIdentifier);
      if (it_ == end_) {
        return node;
      }
      if (it_->value == "[") {
        ConsumeToken(node, it_->value == "[");
        node.children.push_back(ParseExpression());
        ConsumeToken(node, it_->value == "]");
      } else if (it_->value == "(") {
        ConsumeToken(node, it_->value == "(");
        node.children.push_back(ParseExpressionList());
        ConsumeToken(node, it_->value == ")");
      } else if (it_->value == ".") {
        ConsumeToken(node, it_->value == ".");
        ConsumeToken(node, it_->type == TokenType::kIdentifier);
        ConsumeToken(node, it_->value == "(");
        node.children.push_back(ParseExpressionList());
        ConsumeToken(node, it_->value == ")");
      }
      return node;
    }
    // Constants
    ConsumeToken(node, it_->type == TokenType::kIntegerConstant ||
                           it_->type == TokenType::kStringConstant ||
                           it_->value == "true" || it_->value == "false" ||
                           it_->value == "null" || it_->value == "this");
    return node;
  }

  Node ParseExpressionList() {
    // May contain no children (`<expressionList> </expressionList>`).
    Node node(NodeType::kExpressionList);
    if (it_ != end_ && it_->value != ")") {
      node.children.push_back(ParseExpression());
    }
    while (it_ != end_ && it_->value == ",") {
      ConsumeToken(node, it_->value == ",");
      node.children.push_back(ParseExpression());
    }
    return node;
  }

  TokenIterator it_;
  TokenIterator end_;
};

}  // namespace

std::ostream& operator<<(std::ostream& os, NodeType type) {
  switch (type) {
    case NodeType::kClass:
      return os << "class";
    case NodeType::kClassVarDec:
      return os << "classVarDec";
    case NodeType::kSubroutineDec:
      return os << "subroutineDec";
    case NodeType::kParameterList:
      return os << "parameterList";
    case NodeType::kSubroutineBody:
      return os << "subroutineBody";
    case NodeType::kVarDec:
      return os << "varDec";
    case NodeType::kStatements:
      return os << "statements";
    case NodeType::kLetStatement:
      return os << "letStatement";
    case NodeType::kIfStatement:
      return os << "ifStatement";
    case NodeType::kWhileStatement:
      return os << "whileStatement";
    case NodeType::kDoStatement:
      return os << "doStatement";
    case NodeType::kReturnStatement:
      return os << "returnStatement";
    case NodeType::kExpression:
      return os << "expression";
    case NodeType::kTerm:
      return os << "term";
    case NodeType::kExpressionList:
      return os << "expressionList";
  }
  return os << "unknown";
}

Node Parse(const std::vector<Token>& tokens) {
  return Parser(tokens.begin(), tokens.end()).ParseClass();
}

}  // namespace nand2tetris
