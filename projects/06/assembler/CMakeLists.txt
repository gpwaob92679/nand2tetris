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
