#include "instruction.h"

#include <cstdint>

#include "absl/log/log.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_format.h"

namespace nand2tetris {

AInstruction::AInstruction(uint16_t value) : value_(value) {}

std::string AInstruction::ToMachine() const {
  return absl::StrFormat("0%s", value_.to_string());
}

std::string AInstruction::ToAssembly() const {
  return absl::StrFormat("@%u", value_.to_ulong());
}

std::string CInstruction::ToMachine() const {
  return absl::StrFormat("111%s%s%s", computation_.to_string(),
                         destination_.to_string(), jump_.to_string());
}

std::string CInstruction::ToAssembly() const {
  std::string assembly;
  if (destination_.any()) {
    if (destination_[2]) {
      assembly.push_back('A');
    }
    if (destination_[1]) {
      assembly.push_back('D');
    }
    if (destination_[0]) {
      assembly.push_back('M');
    }
    assembly.push_back('=');
  }

  for (const auto& pair : kComputationCodes) {
    if (pair.second == computation_) {
      absl::StrAppend(&assembly, pair.first);
      break;
    }
  }

  if (jump_.any()) {
    assembly.push_back(';');
    absl::StrAppend(&assembly, kJumpCodes[jump_.to_ulong()]);
  }
  return assembly;
}

bool CInstruction::SetComputation(std::string_view computation_str) {
  for (const auto& pair : kComputationCodes) {
    if (pair.first == computation_str) {
      computation_ = pair.second;
      return true;
    }
  }
  return false;
}

bool CInstruction::SetDestination(std::string_view destination_str) {
  for (char c : destination_str) {
    switch (c) {
      case 'A':
        destination_.set(2);
        break;
      case 'D':
        destination_.set(1);
        break;
      case 'M':
        destination_.set(0);
        break;
      default:
        LOG(ERROR) << "Unknown destination " << c;
        return false;
    }
  }
  return true;
}

bool CInstruction::SetJump(std::string_view jump_str) {
  size_t found = std::find(kJumpCodes, kJumpCodes + 8, jump_str) - kJumpCodes;
  if (found == 8) {
    return false;
  }
  jump_ = found;
  return true;
}

}  // namespace nand2tetris
