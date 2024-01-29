message(STATUS "Configuring project 6: Assembler")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

add_library(
  assembler_instruction
  instruction.cpp
)
target_link_libraries(
  assembler_instruction
  absl::log
  absl::strings
  absl::str_format
)

add_executable(
  assembler
  main.cpp
)
target_link_libraries(
  assembler
  assembler_instruction
  absl::check
  absl::log
  absl::log_globals
  absl::log_initialize
  absl::strings
)

install(
  TARGETS assembler
  DESTINATION ${CMAKE_SOURCE_DIR}/projects/bin
)

# Test programs

set(
  test_programs
  ../add/Add.asm
  ../max/Max.asm
  ../max/MaxL.asm
  ../pong/Pong.asm
  ../pong/PongL.asm
  ../rect/Rect.asm
  ../rect/RectL.asm
)

foreach(program ${test_programs})
  get_filename_component(program "${program}" ABSOLUTE)
  file(RELATIVE_PATH identifier "${CMAKE_SOURCE_DIR}" "${program}")
  message(STATUS "Adding test assembly program: ${identifier}")

  get_filename_component(basename "${program}" NAME_WE)
  file(
    COPY "${program}"
    DESTINATION "test_programs/"
  )

  add_test(
    NAME "assembler: Assemble ${identifier}"
    COMMAND assembler "test_programs/${basename}.asm"
  )
  add_test(
    NAME "assembler: Compare ${identifier}"
    COMMAND "${TextComparer}" "test_programs/${basename}.hack" "${CMAKE_CURRENT_SOURCE_DIR}/../expected/${basename}.hack"
  )

  set_tests_properties(
    "assembler: Assemble ${identifier}"
    PROPERTIES
      FIXTURES_SETUP "assembler: ${identifier}"
  )
  set_tests_properties(
    "assembler: Compare ${identifier}"
    PROPERTIES
      FIXTURES_REQUIRED "assembler: ${identifier}"
      PASS_REGULAR_EXPRESSION "Comparison ended successfully"
  )
endforeach()

list(POP_BACK CMAKE_MESSAGE_INDENT)
