#ifndef NAND2TETRIS_ASSEMBLER_INSTRUCTION_H_
#define NAND2TETRIS_ASSEMBLER_INSTRUCTION_H_

#include <bitset>
#include <cstdint>
#include <string>
#include <string_view>
#include <utility>

namespace nand2tetris {

class Instruction {
 public:
  virtual std::string ToMachine() const = 0;
  virtual std::string ToAssembly() const = 0;
};

class AInstruction : public Instruction {
 public:
  AInstruction(uint16_t value);

  std::string ToMachine() const override;
  std::string ToAssembly() const override;

 private:
  std::bitset<15> value_;
};

class CInstruction : public Instruction {
 public:
  std::string ToMachine() const override;
  std::string ToAssembly() const override;

  bool SetComputation(std::string_view computation_str);
  bool SetDestination(std::string_view destination_str);
  bool SetJump(std::string_view jump_str);

 private:
  static constexpr std::pair<std::string_view, std::bitset<7>>
      kComputationCodes[] = {
          {"0", 0b0101010},   {"1", 0b0111111},   {"-1", 0b0111010},
          {"D", 0b0001100},   {"A", 0b0110000},   {"!D", 0b0001101},
          {"!A", 0b0110001},  {"-D", 0b0001111},  {"-A", 0b0110011},
          {"D+1", 0b0011111}, {"A+1", 0b0110111}, {"D-1", 0b0001110},
          {"A-1", 0b0110010}, {"D+A", 0b0000010}, {"D-A", 0b0010011},
          {"A-D", 0b0000111}, {"D&A", 0b0000000}, {"D|A", 0b0010101},
          {"M", 0b1110000},   {"!M", 0b1110001},  {"-M", 0b1110011},
          {"M+1", 0b1110111}, {"M-1", 0b1110010}, {"D+M", 0b1000010},
          {"D-M", 0b1010011}, {"M-D", 0b1000111}, {"D&M", 0b1000000},
          {"D|M", 0b1010101}};
  static constexpr std::string_view kJumpCodes[8] = {
      "", "JGT", "JEQ", "JGE", "JLT", "JNE", "JLE", "JMP"};

  std::bitset<7> computation_;
  std::bitset<3> destination_;
  std::bitset<3> jump_;
};

}  // namespace nand2tetris

#endif  // NAND2TETRIS_ASSEMBLER_INSTRUCTION_H_
