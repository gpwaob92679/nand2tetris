#include <filesystem>
#include <vector>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/flags/usage.h"
#include "absl/log/check.h"
#include "absl/log/globals.h"
#include "absl/log/initialize.h"
#include "absl/log/log.h"
#include "absl/strings/str_format.h"

#include "output.h"
#include "parser.h"
#include "tokenizer.h"

ABSL_FLAG(bool, v, false, "verbose output, print XML output to console");

void Process(std::filesystem::path jack_path) {
  LOG(INFO) << "Processing Jack file: " << jack_path.string();
  std::filesystem::path xml_path = jack_path;
  xml_path.replace_filename(
      absl::StrFormat("%sT.xml", jack_path.stem().string()));
  nand2tetris::XmlFile tokens_xml_file(xml_path.string(),
                                       absl::GetFlag(FLAGS_v));

  std::vector<nand2tetris::Token> tokens =
      nand2tetris::Tokenize(jack_path.string());
  tokens_xml_file << tokens;

  xml_path.replace_filename(
      absl::StrFormat("%s.xml", jack_path.stem().string()));
  nand2tetris::XmlFile class_xml_file(xml_path.string(),
                                      absl::GetFlag(FLAGS_v));
  nand2tetris::Node root = nand2tetris::Parse(tokens);
  class_xml_file << root;
}

int main(int argc, char* argv[]) {
  absl::InitializeLog();
  absl::SetStderrThreshold(absl::LogSeverity::kInfo);

  absl::SetProgramUsageMessage(
      absl::StrFormat("Usage: %s [-v] SOURCE", argv[0]));
  std::vector<char*> positional_args = absl::ParseCommandLine(argc, argv);
  QCHECK_EQ(positional_args.size(), 2) << absl::ProgramUsageMessage();

  std::filesystem::directory_entry source(positional_args[1]);
  if (source.is_directory()) {
    for (const std::filesystem::directory_entry& entry :
         std::filesystem::directory_iterator(source)) {
      if (entry.path().extension() == ".jack") {
        Process(entry.path());
      }
    }
  } else {
    Process(source.path());
  }
  return 0;
}
