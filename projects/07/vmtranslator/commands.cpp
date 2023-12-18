#include "commands.h"

#include <iostream>
#include <string>
#include <string_view>

#include "absl/strings/str_cat.h"
#include "absl/strings/str_format.h"

std::ostream &operator<<(std::ostream &os, const Command &command) {
  return os << command.ToAssembly();
}

BinaryArithmeticCommand::BinaryArithmeticCommand(std::string_view write_command)
    : write_command_(write_command) {}

std::string BinaryArithmeticCommand::ToAssembly() const {
  return absl::StrFormat(
      "@SP\n"
      "AM=M-1\n"
      "D=M\n"
      "A=A-1\n"
      "%s\n",
      write_command_);
}

AddCommand::AddCommand() : BinaryArithmeticCommand("M=D+M") {}
SubCommand::SubCommand() : BinaryArithmeticCommand("M=M-D") {}
AndCommand::AndCommand() : BinaryArithmeticCommand("M=D&M") {}
OrCommand::OrCommand() : BinaryArithmeticCommand("M=D|M") {}

UnaryArithmeticCommand::UnaryArithmeticCommand(std::string_view write_command)
    : write_command_(write_command) {}

std::string UnaryArithmeticCommand::ToAssembly() const {
  return absl::StrFormat(
      "@SP\n"
      "A=M-1\n"
      "%s\n",
      write_command_);
}

NegCommand::NegCommand() : UnaryArithmeticCommand("M=-M") {}
NotCommand::NotCommand() : UnaryArithmeticCommand("M=!M") {}

std::string BinaryComparisonCommand::ToAssembly() const {
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
      else_label_, jump_condition_, end_label_, else_label_, end_label_);
}

BinaryComparisonCommand::BinaryComparisonCommand(
    std::string_view jump_condition, std::string_view else_label,
    std::string_view end_label)
    : jump_condition_(jump_condition),
      else_label_(else_label),
      end_label_(end_label) {}

EqCommand::EqCommand(std::string_view label)
    : BinaryComparisonCommand("JNE", absl::StrCat(label, "$eq_else"),
                              absl::StrCat(label, "$eq_end")) {}
GtCommand::GtCommand(std::string_view label)
    : BinaryComparisonCommand("JLE", absl::StrCat(label, "$gt_else"),
                              absl::StrCat(label, "$gt_end")) {}
LtCommand::LtCommand(std::string_view label)
    : BinaryComparisonCommand("JGE", absl::StrCat(label, "$lt_else"),
                              absl::StrCat(label, "$lt_end")) {}
