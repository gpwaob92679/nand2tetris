#ifndef NAND2TETRIS_VMTRANSLATOR_COMMANDS_H_
#define NAND2TETRIS_VMTRANSLATOR_COMMANDS_H_

#include <iostream>
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

#endif  // NAND2TETRIS_VMTRANSLATOR_COMMANDS_H_
