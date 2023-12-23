#include "parser.h"

#include <filesystem>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "absl/log/check.h"
#include "absl/log/log.h"
#include "absl/strings/str_format.h"
#include "absl/strings/str_split.h"

#include "addressing.h"
#include "commands.h"

VmFile::VmFile(std::string_view filename)
    : filename_(std::filesystem::path(filename).stem().string()),
      file_(filename.data()) {
  QCHECK(file_.is_open()) << "Could not open file: " << filename;
  Advance();
}

VmFile::~VmFile() {
  file_.close();
  delete command_;
}

std::unique_ptr<Address> VmFile::ParseAddress(std::string_view segment,
                                              std::string_view index_str) {
  uint16_t index = std::stoi(index_str.data());
  if (segment == "argument") {
    return std::make_unique<ArgumentAddress>(index);
  }
  if (segment == "local") {
    return std::make_unique<LocalAddress>(index);
  }
  if (segment == "static") {
    return std::make_unique<StaticAddress>(filename_, index);
  }
  if (segment == "constant") {
    return std::make_unique<ConstantAddress>(index);
  }
  if (segment == "this") {
    return std::make_unique<ThisAddress>(index);
  }
  if (segment == "that") {
    return std::make_unique<ThatAddress>(index);
  }
  if (segment == "pointer") {
    return std::make_unique<PointerAddress>(index);
  }
  if (segment == "temp") {
    return std::make_unique<TempAddress>(index);
  }
  LOG(ERROR) << "Invalid address: " << segment << ' ' << index;
  return nullptr;
}

void VmFile::Advance() {
  if (command_) {
    line_.clear();
    delete command_;
    command_ = nullptr;
  }

  std::vector<std::string_view> tokens;
  while (tokens.empty() && std::getline(file_, line_)) {
    std::string_view line_view = line_;
    ++line_number_;
    line_view = line_view.substr(0, line_view.find("//"));
    tokens =
        absl::StrSplit(line_view, absl::ByAnyChar(" \t"), absl::SkipEmpty());
  }
  if (tokens.empty()) {
    return;
  }

  if (tokens[0] == "add") {
    command_ = new AddCommand();
  } else if (tokens[0] == "sub") {
    command_ = new SubCommand();
  } else if (tokens[0] == "neg") {
    command_ = new NegCommand();
  } else if (tokens[0] == "eq") {
    command_ = new EqCommand(absl::StrFormat("%s_%d", filename_, line_number_));
  } else if (tokens[0] == "gt") {
    command_ = new GtCommand(absl::StrFormat("%s_%d", filename_, line_number_));
  } else if (tokens[0] == "lt") {
    command_ = new LtCommand(absl::StrFormat("%s_%d", filename_, line_number_));
  } else if (tokens[0] == "and") {
    command_ = new AndCommand();
  } else if (tokens[0] == "or") {
    command_ = new OrCommand();
  } else if (tokens[0] == "not") {
    command_ = new NotCommand();
  } else if (tokens[0] == "push") {
    QCHECK_EQ(tokens.size(), 3) << filename_ << ':' << line_number_
                                << ": Invalid push command: " << line_;
    command_ = new PushCommand(ParseAddress(tokens[1], tokens[2]));
  } else if (tokens[0] == "pop") {
    QCHECK_EQ(tokens.size(), 3) << filename_ << ':' << line_number_
                                << ": Invalid pop command: " << line_;
    command_ = new PopCommand(ParseAddress(tokens[1], tokens[2]));
  } else {
    LOG(ERROR) << filename_ << ':' << line_number_
               << ": Unknown command: " << line_;
  }
}

std::string VmFile::line() { return line_; }
Command *VmFile::command() { return command_; }
