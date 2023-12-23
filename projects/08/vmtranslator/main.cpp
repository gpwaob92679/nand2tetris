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

  VmFile vm_file(positional_args[1]);
  std::ofstream asm_file(
      absl::StrCat(absl::StripSuffix(positional_args[1], ".vm"), ".asm"));

  while (vm_file.command()) {
    if (absl::GetFlag(FLAGS_v)) {
      DLOG(INFO) << vm_file.line() << " ->\n"
                 << vm_file.command()->ToAssembly();
    }
    asm_file << vm_file.command()->ToAssembly();
    vm_file.Advance();
  }

  asm_file << "(END)\n"
           << "@END\n"
           << "0;JMP\n";
  return 0;
}
