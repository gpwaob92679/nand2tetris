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

void VmFile::Advance() {
  if (command_) {
    delete command_;
    command_ = nullptr;
  }

  std::string line;
  std::vector<std::string_view> tokens;
  while (tokens.empty() && std::getline(file_, line)) {
    std::string_view line_view = line;
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
                                << ": Invalid push command: " << line;
    uint16_t index = std::stoi(tokens[2].data());
    if (tokens[1] == "argument") {
      command_ = new PushCommand(std::make_unique<ArgumentAddress>(index));
    } else if (tokens[1] == "local") {
      command_ = new PushCommand(std::make_unique<LocalAddress>(index));
    } else if (tokens[1] == "static") {
      command_ =
          new PushCommand(std::make_unique<StaticAddress>(filename_, index));
    } else if (tokens[1] == "constant") {
      command_ = new PushCommand(std::make_unique<ConstantAddress>(index));
    } else if (tokens[1] == "this") {
      command_ = new PushCommand(std::make_unique<ThisAddress>(index));
    } else if (tokens[1] == "that") {
      command_ = new PushCommand(std::make_unique<ThatAddress>(index));
    } else if (tokens[1] == "pointer") {
      command_ = new PushCommand(std::make_unique<PointerAddress>(index));
    } else if (tokens[1] == "temp") {
      command_ = new PushCommand(std::make_unique<TempAddress>(index));
    } else {
      LOG(ERROR) << filename_ << ':' << line_number_
                 << ": Unknown segment: " << tokens[1];
    }
  } else if (tokens[0] == "pop") {
  } else {
    LOG(ERROR) << filename_ << ':' << line_number_
               << ": Unknown command: " << line;
  }
}

Command *VmFile::command() { return command_; }
