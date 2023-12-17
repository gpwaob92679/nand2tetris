#include "addressing.h"

#include <string>
#include <string_view>

#include "absl/log/log.h"
#include "absl/strings/str_format.h"

std::string DestinationString(uint16_t destination) {
  std::string result;
  if (destination & Destination::kA) {
    result.push_back('A');
  }
  if (destination & Destination::kD) {
    result.push_back('D');
  }
  if (destination & Destination::kM) {
    result.push_back('M');
  }
  return result;
};

StaticAddress::StaticAddress(std::string_view class_name, uint16_t index)
    : class_name_(class_name), index_(index) {}

std::string StaticAddress::AddressingAssembly(uint16_t destination) const {
  return absl::StrFormat(
      "@%s.%u\n"
      "%s=M\n",
      class_name_, index_, DestinationString(destination));
}

ConstantAddress::ConstantAddress(uint16_t index) : index_(index) {}

std::string ConstantAddress::AddressingAssembly(uint16_t destination) const {
  std::string result = absl::StrFormat("@%u\n", index_);
  destination = destination & ~Destination::kA;
  if (destination) {
    absl::StrAppendFormat(&result, "%s=A\n", DestinationString(destination));
  }
  return result;
}

PointerAddress::PointerAddress(uint16_t index) : ConstantAddress(3 + index) {
  if (index >= 2) {
    LOG(WARNING) << "Index out of range: " << index
                 << " (pointer segment is 2 words long)";
  }
}

TempAddress::TempAddress(uint16_t index) : ConstantAddress(5 + index) {
  if (index >= 8) {
    LOG(WARNING) << "Index out of range: " << index
                 << " (temp segment is 8 words long)";
  }
}
