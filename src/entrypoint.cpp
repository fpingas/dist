#include <dist/entrypoint.hpp>
#include <dist/execution.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

namespace dist {
void run_script(char *script) {
  std::ifstream input_file_stream(script);
  std::stringstream buffer;
  buffer << input_file_stream.rdbuf();
  dist::run(buffer.str());
}

void run_repl() {
  std::string line;
  while (std::getline(std::cin, line)) {
    dist::run(line);
  }
}
} // namespace dist