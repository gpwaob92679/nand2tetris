message(STATUS "Configuring project 6: Assembler")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

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
    NAME "assembler: Assemble ${program}"
    COMMAND assembler "test_programs/${program_stem}.asm"
  )
  add_test(
    NAME "assembler: Compare ${program}"
    COMMAND "${TextComparer}" "test_programs/${program_stem}.hack" "${CMAKE_CURRENT_SOURCE_DIR}/../expected/${program_stem}.hack"
  )

  set_tests_properties(
    "assembler: Assemble ${program}"
    PROPERTIES
      FIXTURES_SETUP "assembler: ${program}"
  )
  set_tests_properties(
    "assembler: Compare ${program}"
    PROPERTIES
      FIXTURES_REQUIRED "assembler: ${program}"
      PASS_REGULAR_EXPRESSION "Comparison ended successfully"
  )
endforeach()

list(POP_BACK CMAKE_MESSAGE_INDENT)
