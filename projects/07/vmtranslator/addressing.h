#ifndef NAND2TETRIS_VMTRANSLATOR_ADDRESSING_H_
#define NAND2TETRIS_VMTRANSLATOR_ADDRESSING_H_

#include <string>
#include <string_view>

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

class PointerAddressedAddress : public Address {
 public:
  PointerAddressedAddress(std::string_view pointer, uint16_t index);
  std::string AddressingAssembly(uint16_t destination) const;

 private:
  std::string pointer_;
  uint16_t index_;
};

class ArgumentAddress : public PointerAddressedAddress {
 public:
  ArgumentAddress(uint16_t index);
};

class LocalAddress : public PointerAddressedAddress {
 public:
  LocalAddress(uint16_t index);
};

class ThisAddress : public PointerAddressedAddress {
 public:
  ThisAddress(uint16_t index);
};

class ThatAddress : public PointerAddressedAddress {
 public:
  ThatAddress(uint16_t index);
};

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
