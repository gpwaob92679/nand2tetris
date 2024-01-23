#include <filesystem>
#include <vector>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/flags/usage.h"
#include "absl/log/check.h"
#include "absl/log/log.h"
#include "absl/strings/str_format.h"

#include "output.h"
#include "tokenizer.h"

ABSL_FLAG(bool, v, false, "verbose output, print XML output to console");

void Tokenize(std::filesystem::path jack_path) {
  LOG(INFO) << "Processing Jack file: " << jack_path.string();
  std::filesystem::path xml_path = jack_path;
  xml_path.replace_filename(
      absl::StrFormat("%sT.xml", jack_path.stem().string()));
  nand2tetris::TokensXmlFile xml_file(xml_path.string());

  for (auto& token : nand2tetris::Tokenize(jack_path.string())) {
    if (absl::GetFlag(FLAGS_v)) {
      LOG(INFO) << token->ToXmlElement();
    }
    xml_file << *token;
  }
}

int main(int argc, char* argv[]) {
  absl::SetProgramUsageMessage(
      absl::StrFormat("Usage: %s [-v] SOURCE", argv[0]));
  std::vector<char*> positional_args = absl::ParseCommandLine(argc, argv);
  QCHECK_EQ(positional_args.size(), 2) << absl::ProgramUsageMessage();

  std::filesystem::directory_entry source(positional_args[1]);
  if (source.is_directory()) {
    for (const std::filesystem::directory_entry& entry :
         std::filesystem::directory_iterator(source)) {
      if (entry.path().extension() == ".jack") {
        Tokenize(entry.path());
      }
    }
  } else {
    Tokenize(source.path());
  }
  return 0;
}
