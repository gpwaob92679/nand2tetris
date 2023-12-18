#include "absl/log/check.h"
#include "absl/log/log.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/strip.h"

#include "commands.h"
#include "parser.h"

int main(int argc, char *argv[]) {
  QCHECK_GE(argc, 2) << "Usage: vmtranslator INPUT_VM_FILE";

  VmFile vm_file(argv[1]);
  std::ofstream asm_file(
      absl::StrCat(absl::StripSuffix(argv[1], ".vm"), ".asm"));
  for (Command *command = vm_file.command(); command;
       vm_file.Advance(), command = vm_file.command()) {
    DLOG(INFO) << *command;
    asm_file << command->ToAssembly();
  }

  asm_file << "(END)\n"
           << "@END\n"
           << "0;JMP\n";
  return 0;
}
