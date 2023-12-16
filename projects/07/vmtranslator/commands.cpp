#include "commands.h"

#include <iostream>

std::ostream &operator<<(std::ostream &os, const Command &command) {
  return os << command.ToString();
}
