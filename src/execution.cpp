#include <dist/execution.hpp>
#include <dist/lexing.hpp>
#include <iostream>

namespace dist {
void run(std::string_view code) {
  auto tokens = scan_tokens(code);
  for (auto token : tokens) {
    std::cout << "type:\n"
              << static_cast<std::underlying_type<dist::token_type>::type>(token.type) << "\nlexeme:\n"
              << token.lexeme << "\npos:\n"
              << token.pos << "\nline:\n"
              << token.line << std::endl;
  }
}
} // namespace dist