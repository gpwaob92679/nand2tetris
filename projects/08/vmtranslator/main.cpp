#include <filesystem>
#include <iostream>
#include <vector>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/flags/usage.h"
#include "absl/log/check.h"
#include "absl/log/log.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_format.h"
#include "absl/strings/strip.h"

#include "commands.h"
#include "parser.h"

ABSL_FLAG(bool, v, false, "Output assembly code to console");

int main(int argc, char *argv[]) {
  absl::SetProgramUsageMessage(
      absl::StrFormat("Usage: %s [-v] SOURCE", argv[0]));
  std::vector<char *> positional_args = absl::ParseCommandLine(argc, argv);
  QCHECK_EQ(positional_args.size(), 2) << absl::ProgramUsageMessage();

  std::filesystem::directory_entry source(positional_args[1]);
  QCHECK(source.exists()) << "File or directory does not exist: "
                          << positional_args[1];
  LOG(INFO) << "Source: " << source.path();

  std::string program_name;
  std::filesystem::path asm_path;
  if (source.is_directory()) {
    if (source.path().has_filename()) {
      program_name = source.path().filename().string();
    } else {
      program_name = source.path().parent_path().filename().string();
    }
    asm_path = source.path() / absl::StrCat(program_name, ".asm");
  } else {
    program_name = source.path().stem().string();
    asm_path = source.path().parent_path() / absl::StrCat(program_name, ".asm");
  }
  CHECK(!program_name.empty());
  LOG(INFO) << "Program: " << program_name;
  LOG(INFO) << "Output: " << asm_path;

  std::ofstream asm_file(asm_path);
  CHECK(asm_file.is_open()) << "Could not open output file: " << asm_path;
  // Bootstrap code.
  asm_file << "@256\n"
           << "D=A\n"
           << "@SP\n"
           << "M=D\n";

  if (source.is_directory()) {
    // Although `Sys.init` is expected to enter an infinite loop, we still add
    // an infinite loop in case `Sys.init` returns.
    asm_file << CallCommand("Sys.init", 0, "END").ToAssembly();
    asm_file << "@END\n"
             << "(END)\n"
             << "0;JMP\n";

    for (const std::filesystem::directory_entry &entry :
         std::filesystem::directory_iterator(source)) {
      if (entry.path().extension() == ".vm") {
        LOG(INFO) << "Translating: " << entry.path();
        VmFile vm_file(entry.path().string());
        while (vm_file.command()) {
          if (absl::GetFlag(FLAGS_v)) {
            LOG(INFO) << vm_file.line() << " ->\n"
                      << vm_file.command()->ToAssembly();
          }
          asm_file << vm_file.command()->ToAssembly();
          vm_file.Advance();
        }
      }
    }
  } else {
    VmFile vm_file(positional_args[1]);
    while (vm_file.command()) {
      if (absl::GetFlag(FLAGS_v)) {
        LOG(INFO) << vm_file.line() << " ->\n"
                  << vm_file.command()->ToAssembly();
      }
      asm_file << vm_file.command()->ToAssembly();
      vm_file.Advance();
    }
    asm_file << "@END\n"
             << "(END)\n"
             << "0;JMP\n";
  }
  return 0;
}
