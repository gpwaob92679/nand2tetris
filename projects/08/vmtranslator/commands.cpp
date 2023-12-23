#include "commands.h"

#include <iostream>
#include <memory>
#include <string>
#include <string_view>

#include "absl/strings/str_cat.h"
#include "absl/strings/str_format.h"

#include "addressing.h"

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

PushCommand::PushCommand(std::unique_ptr<Address> address)
    : address_(std::move(address)) {}

std::string PushCommand::ToAssembly() const {
  return absl::StrFormat(
      "%s"
      "D=%c\n"
      "@SP\n"
      "A=M\n"
      "M=D\n"
      "@SP\n"
      "M=M+1\n",
      address_->AddressingAssembly(Destination::kA),
      address_->value_register());
}

PopCommand::PopCommand(std::unique_ptr<Address> address)
    : address_(std::move(address)) {}

std::string PopCommand::ToAssembly() const {
  if (dynamic_cast<PointerAddressedAddress *>(address_.get())) {
    return absl::StrFormat(
        "%s"
        "@R15\n"
        "M=D\n"
        "@SP\n"
        "AM=M-1\n"
        "D=M\n"
        "@R15\n"
        "A=M\n"
        "M=D\n",
        address_->AddressingAssembly(Destination::kD));
  }
  return absl::StrFormat(
      "@SP\n"
      "AM=M-1\n"
      "D=M\n"
      "%s"
      "M=D\n",
      address_->AddressingAssembly(Destination::kA));
}

LabelCommand::LabelCommand(std::string_view label) : label_(label) {}

std::string LabelCommand::ToAssembly() const {
  return absl::StrFormat("(%s)\n", label_);
}

GotoCommand::GotoCommand(std::string_view label) : label_(label) {}

std::string GotoCommand::ToAssembly() const {
  return absl::StrFormat(
      "@%s\n"
      "0;JMP\n",
      label_);
}

IfGotoCommand::IfGotoCommand(std::string_view label) : label_(label) {}

std::string IfGotoCommand::ToAssembly() const {
  return absl::StrFormat(
      "@SP\n"
      "AM=M-1\n"
      "D=M\n"
      "@%s\n"
      "D;JNE\n",
      label_);
}
