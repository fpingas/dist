#include <dist/entrypoint.hpp>
#include <iostream>

int main(int argc, char *argv[]) {
  try {
    if (argc > 1) {
      dist::run_script(argv[1]);
    } else {
      dist::run_repl();
    }
  } catch (const std::exception &e) {
    std::cout << "Expection caught on main: " << e.what() << std::endl;
    throw(e);
  }
  return 0;
}