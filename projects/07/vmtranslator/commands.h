#ifndef NAND2TETRIS_VMTRANSLATOR_COMMANDS_H_
#define NAND2TETRIS_VMTRANSLATOR_COMMANDS_H_

#include <iostream>
#include <string>
#include <string_view>

#include "absl/strings/str_format.h"

class Command {
 public:
  virtual std::string ToString() const = 0;
};
std::ostream &operator<<(std::ostream &os, const Command &command);

template <class Op>
class BinaryArithmeticCommand : public Command {
 public:
  std::string ToString() const override {
    return absl::StrFormat(
        "@SP\n"
        "AM=M-1\n"
        "D=M\n"
        "A=A-1\n"
        "%s\n",
        Op::command);
  }
};

struct Add {
  static constexpr std::string_view command = "M=D+M";
};
using AddCommand = BinaryArithmeticCommand<Add>;

struct Sub {
  static constexpr std::string_view command = "M=M-D";
};
using SubCommand = BinaryArithmeticCommand<Sub>;

struct And {
  static constexpr std::string_view command = "M=D&M";
};
using AndCommand = BinaryArithmeticCommand<And>;

struct Or {
  static constexpr std::string_view command = "M=D|M";
};
using OrCommand = BinaryArithmeticCommand<Or>;

template <class Op>
class UnaryArithmeticCommand : public Command {
 public:
  std::string ToString() const {
    return absl::StrFormat(
        "@SP\n"
        "A=M-1\n"
        "%s\n",
        Op::command);
  }
};

struct Neg {
  static constexpr std::string_view command = "M=-M";
};
using NegCommand = UnaryArithmeticCommand<Neg>;

struct Not {
  static constexpr std::string_view command = "M=!M";
};
using NotCommand = UnaryArithmeticCommand<Not>;

template <class Op>
class BinaryComparisonCommand : public Command {
 public:
  BinaryComparisonCommand(std::string_view else_label,
                          std::string_view end_label)
      : else_label_(else_label), end_label_(end_label) {}

  std::string ToString() const {
    return absl::StrFormat(
        "@SP\n"
        "AM=M-1\n"
        "D=M\n"
        "A=A-1\n"
        "D=M-D\n"
        "@%s\n"
        "D;%s\n"
        "@SP\n"
        "A=M-1\n"
        "M=-1\n"
        "@%s\n"
        "0;JMP\n"
        "(%s)\n"
        "@SP\n"
        "A=M-1\n"
        "M=0\n"
        "(%s)\n",
        else_label_, Op::jump_condition, end_label_, else_label_, end_label_);
  }

 private:
  std::string else_label_, end_label_;
};

struct Eq {
  static constexpr std::string_view jump_condition = "JNE";
};
using EqCommand = BinaryComparisonCommand<Eq>;

struct Gt {
  static constexpr std::string_view jump_condition = "JLE";
};
using GtCommand = BinaryComparisonCommand<Gt>;

struct Lt {
  static constexpr std::string_view jump_condition = "JGE";
};
using LtCommand = BinaryComparisonCommand<Lt>;

#endif  // NAND2TETRIS_VMTRANSLATOR_COMMANDS_H_
