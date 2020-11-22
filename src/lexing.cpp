#include <algorithm>
#include <dist/lexing.hpp>
#include <tuple>

namespace {
bool end_of_code(std::string_view code) { return code.empty(); }

std::string_view step(std::string_view code, size_t step_size) { return code.substr(step_size); }

bool is_next_char(char expected, std::string_view code) { return !code.empty() && code.front() == expected; }

size_t get_comment_size(std::string_view code) {
  auto newline_or_npos = code.find_first_of('\n');
  return newline_or_npos == std::string_view::npos ? code.size() : newline_or_npos + 1;
}

std::pair<size_t, size_t> get_space_size(std::string_view code) {
  auto nonspace_or_npos = code.find_first_not_of(" \n\t\r\v\f");
  auto whitespace_view = code.substr(0, nonspace_or_npos);
  auto newline_count = std::count(whitespace_view.begin(), whitespace_view.end(), '\n');
  return nonspace_or_npos == std::string_view::npos ? std::pair(code.size(), newline_count)
                                                    : std::pair(nonspace_or_npos, newline_count);
}

std::tuple<dist::token_type, size_t, size_t> scan_token(std::string_view code) {
  switch (code[0]) {
  case '(':
    return std::tuple(dist::token_type::tok_left_paren, 1, 0);
  case ')':
    return std::tuple(dist::token_type::tok_right_paren, 1, 0);
  case '{':
    return std::tuple(dist::token_type::tok_left_brace, 1, 0);
  case '}':
    return std::tuple(dist::token_type::tok_right_brace, 1, 0);
  case ',':
    return std::tuple(dist::token_type::tok_comma, 1, 0);
  case '.':
    return std::tuple(dist::token_type::tok_dot, 1, 0);
  case '-':
    return std::tuple(dist::token_type::tok_minus, 1, 0);
  case '+':
    return std::tuple(dist::token_type::tok_plus, 1, 0);
  case ';':
    return std::tuple(dist::token_type::tok_semicolon, 1, 0);
  case '*':
    return std::tuple(dist::token_type::tok_star, 1, 0);
  case '!':
    if (is_next_char('=', code.substr(1))) {
      return std::tuple(dist::token_type::tok_bang_equal, 2, 0);
    } else {
      return std::tuple(dist::token_type::tok_bang, 1, 0);
    }
  case '=':
    if (is_next_char('=', code.substr(1))) {
      return std::tuple(dist::token_type::tok_equal_equal, 2, 0);
    } else {
      return std::tuple(dist::token_type::tok_equal, 1, 0);
    }
  case '<':
    if (is_next_char('=', code.substr(1))) {
      return std::tuple(dist::token_type::tok_less_equal, 2, 0);
    } else {
      return std::tuple(dist::token_type::tok_less, 1, 0);
    }
  case '>':
    if (is_next_char('=', code.substr(1))) {
      return std::tuple(dist::token_type::tok_greater_equal, 2, 0);
    } else {
      return std::tuple(dist::token_type::tok_greater, 1, 0);
    }
  case '/':
    if (is_next_char('/', code.substr(1))) {
      size_t comment_size = get_comment_size(code);
      return std::tuple(dist::token_type::tok_comment, comment_size, 1);
    } else {
      return std::tuple(dist::token_type::tok_slash, 1, 0);
    }
  case ' ':
  case '\n':
  case '\t':
  case '\r':
  case '\v':
  case '\f': {
    auto [space_size, newline_count] = get_space_size(code);
    return std::tuple(dist::token_type::tok_space, space_size, newline_count);
  }
  case '"':
    // TODO
  default:
    return std::tuple(dist::token_type::tok_invalid_token, 0, 0);
  }
}
} // namespace

namespace dist {
std::vector<dist::token> scan_tokens(std::string_view code) {
  std::vector<dist::token> tokens;
  size_t curr_pos_in_line = 0;
  size_t curr_line = 0;
  while (!end_of_code(code)) {
    auto [token_type, token_size, newline_count] = scan_token(code);
    auto token = dist::token(token_type, code.substr(0, token_size), curr_pos_in_line, curr_line);
    curr_pos_in_line += token_size;
    if (newline_count) {
      curr_pos_in_line = 0;
      curr_line += newline_count;
    }
    tokens.emplace_back(token);
    if (token_type == dist::token_type::tok_invalid_token) {
      return tokens;
    }
    code = step(code, token_size);
  }
  tokens.emplace_back(dist::token(dist::token_type::tok_eof, "", curr_pos_in_line, curr_line));
  return tokens;
}
} // namespace dist
