#ifndef NAND2TETRIS_VMTRANSLATOR_ADDRESSING_H_
#define NAND2TETRIS_VMTRANSLATOR_ADDRESSING_H_

#include <string>
#include <string_view>

#include "absl/strings/str_format.h"

enum Destination {
  kA = 4,
  kD = 2,
  kM = 1,
};

std::string DestinationString(uint16_t destination);

class Address {
 public:
  virtual std::string AddressingAssembly(uint16_t destination) const = 0;
};

template <class Segment>
class PointerAddressedAddress : public Address {
 public:
  PointerAddressedAddress(uint16_t index) : index_(index) {}
  std::string AddressingAssembly(uint16_t destination) const {
    return absl::StrFormat(
        "@%u\n"
        "D=A\n"
        "@%s\n"
        "%s=D+M\n",
        index_, Segment::pointer, DestinationString(destination));
  }

 private:
  uint16_t index_;
};

struct Argument {
  static constexpr std::string_view pointer = "ARG";
};
using ArgumentAddress = PointerAddressedAddress<Argument>;

struct Local {
  static constexpr std::string_view pointer = "LCL";
};
using LocalAddress = PointerAddressedAddress<Local>;

struct This {
  static constexpr std::string_view pointer = "THIS";
};
using ThisAddress = PointerAddressedAddress<This>;

struct That {
  static constexpr std::string_view pointer = "THAT";
};
using ThatAddress = PointerAddressedAddress<That>;

class StaticAddress : public Address {
 public:
  StaticAddress(std::string_view class_name, uint16_t index);
  std::string AddressingAssembly(uint16_t destination) const override;

 private:
  std::string class_name_;
  uint16_t index_;
};

class ConstantAddress : public Address {
 public:
  ConstantAddress(uint16_t index);
  std::string AddressingAssembly(uint16_t destination) const override;

 private:
  uint16_t index_;
};

class PointerAddress : public ConstantAddress {
 public:
  PointerAddress(uint16_t index);
};

class TempAddress : public ConstantAddress {
 public:
  TempAddress(uint16_t index);
};

#endif  // NAND2TETRIS_VMTRANSLATOR_ADDRESSING_H_
