#ifndef DIST_TOKEN_H
#define DIST_TOKEN_H

#include <string_view>

namespace dist {
enum class token_type {
  tok_left_paren,
  tok_right_paren,
  tok_left_brace,
  tok_right_brace,
  tok_comma,
  tok_dot,
  tok_minus,
  tok_plus,
  tok_semicolon,
  tok_slash,
  tok_star,
  tok_bang,
  tok_bang_equal,
  tok_equal,
  tok_equal_equal,
  tok_greater,
  tok_greater_equal,
  tok_less,
  tok_less_equal,
  tok_identifier,
  tok_string,
  tok_number,
  tok_and,
  tok_class,
  tok_else,
  tok_false,
  tok_fun,
  tok_for,
  tok_if,
  tok_null,
  tok_or,
  tok_print,
  tok_return,
  tok_super,
  tok_this,
  tok_true,
  tok_var,
  tok_while,
  tok_eof,
  tok_invalid_token,
  tok_comment,
  tok_space
};

struct token {
  token(dist::token_type type, std::string_view lexeme, size_t pos, size_t line)
      : type(type), lexeme(lexeme), pos(pos), line(line) {}
  const dist::token_type type;
  const std::string_view lexeme;
  const size_t pos;
  const size_t line;
};
} // namespace dist

#endif