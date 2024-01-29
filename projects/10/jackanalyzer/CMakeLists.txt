message(STATUS "Configuring project 10: Jack analyzer")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

add_library(
  jackanalyzer_tokenizer
  tokenizer.cpp
)
target_link_libraries(
  jackanalyzer_tokenizer
  absl::charset
  absl::check
  absl::log
  absl::str_format
  absl::strings
)

add_library(
  jackanalyzer_parser
  parser.cpp
)
target_link_libraries(
  jackanalyzer_parser
  absl::charset
  absl::check
  absl::log
  jackanalyzer_tokenizer
)

add_library(
  jackanalyzer_output
  output.cpp
)
target_link_libraries(
  jackanalyzer_output
  absl::check
  absl::log
  absl::str_format
  absl::strings
  jackanalyzer_parser
  jackanalyzer_tokenizer
)

add_executable(
  jackanalyzer
  main.cpp
)
target_link_libraries(
  jackanalyzer
  absl::check
  absl::flags
  absl::flags_parse
  absl::flags_usage
  absl::log
  absl::log_globals
  absl::log_initialize
  absl::str_format
  jackanalyzer_output
  jackanalyzer_parser
  jackanalyzer_tokenizer
)

install(
  TARGETS jackanalyzer
  DESTINATION ${CMAKE_SOURCE_DIR}/projects/bin
)

# Test programs

set(
  test_programs
  ../ArrayTest/
  ../ExpressionLessSquare/
  ../Square/
)

foreach(program ${test_programs})
  get_filename_component(program "${program}" ABSOLUTE)
  file(RELATIVE_PATH identifier "${CMAKE_SOURCE_DIR}" "${program}")
  message(STATUS "Adding test Jack program: ${identifier}")

  get_filename_component(basename "${program}" NAME_WE)
  message(DEBUG "basename: " ${basename})
  file(
    COPY "${program}/"
    DESTINATION "test_programs/${basename}/"
    PATTERN "*.vm" EXCLUDE
  )

  add_test(
    NAME "jackanalyzer: Tokenize ${identifier}"
    COMMAND jackanalyzer "test_programs/${basename}/"
  )
  set_tests_properties(
    "jackanalyzer: Tokenize ${identifier}"
    PROPERTIES
      FIXTURES_SETUP "jackanalyzer: ${identifier}"
  )

  file(GLOB xml_files "${program}/expected/*.xml")
  list(APPEND CMAKE_MESSAGE_INDENT "  ")
  foreach(xml_file ${xml_files})
    get_filename_component(xml_filename "${xml_file}" NAME)
    message(DEBUG "Found expected XML file: ${xml_filename}")
    add_test(
      NAME "jackanalyzer: Compare ${identifier}/${xml_filename}"
      COMMAND "${TextComparer}" "${xml_file}" "test_programs/${basename}/${xml_filename}"
    )
    set_tests_properties(
      "jackanalyzer: Compare ${identifier}/${xml_filename}"
      PROPERTIES
        FIXTURES_REQUIRED "jackanalyzer: ${identifier}"
        PASS_REGULAR_EXPRESSION "Comparison ended successfully"
    )
  endforeach()
  list(POP_BACK CMAKE_MESSAGE_INDENT)
endforeach()

list(POP_BACK CMAKE_MESSAGE_INDENT)
