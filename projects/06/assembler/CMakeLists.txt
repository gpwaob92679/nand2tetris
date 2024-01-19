add_executable(
  assembler
  main.cpp
)
target_link_libraries(
  assembler
  absl::check
  absl::log
  absl::strings
)

install(
  TARGETS assembler
  DESTINATION ${CMAKE_SOURCE_DIR}/projects/bin
)

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
  message(STATUS "Adding test assembly program: " ${program})
  file(
    COPY ${program}
    DESTINATION test_programs/
  )

  cmake_path(GET program STEM program_stem)
  add_test(
    NAME "Assemble: ${program}"
    COMMAND assembler "test_programs/${program_stem}.asm"
  )
  add_test(
    NAME "Comparison: ${program}"
    COMMAND "${TextComparer}" "test_programs/${program_stem}.hack" "${CMAKE_CURRENT_SOURCE_DIR}/../expected/${program_stem}.hack"
  )

  set_tests_properties(
    "Assemble: ${program}"
    PROPERTIES
      FIXTURES_SETUP "${program}"
  )
  set_tests_properties(
    "Comparison: ${program}"
    PROPERTIES
      FIXTURES_REQUIRED "${program}"
      PASS_REGULAR_EXPRESSION "Comparison ended successfully"
  )
endforeach()
