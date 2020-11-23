#include <algorithm>
#include <dist/lexing.hpp>
#include <re2/re2.h>
#include <tuple>
#include <unordered_map>

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

std::tuple<bool, size_t, size_t> get_str_literal_size(std::string_view code) {
  std::string match;
  if (RE2::PartialMatch(code, R"(("(\\.|[^\\\"])*"))", &match)) {
    auto newline_count = std::count(match.begin(), match.end(), '\n');
    return std::tuple(true, match.size(), newline_count);
  } else {
    return std::tuple(false, code.size(), 0);
  }
}

size_t get_number_literal_size(std::string_view code) {
  std::string match;
  RE2::PartialMatch(code, R"((([0-9]+[.])?[0-9]+))", &match);
  return match.size();
}

bool is_alpha(char c) { return isalpha(c) or c == '_'; }

size_t get_identifier_size(std::string_view code) {
  auto it = std::find_if_not(code.begin(), code.end(), [](char c) { return is_alpha(c) or isdigit(c); });
  return it - code.begin();
}

const std::unordered_map<std::string_view, dist::token_type> &reserved_words() {
  static const std::unordered_map<std::string_view, dist::token_type> reserved_words = {
      {"and", dist::token_type::tok_and},       {"class", dist::token_type::tok_class},
      {"else", dist::token_type::tok_else},     {"false", dist::token_type::tok_false},
      {"for", dist::token_type::tok_for},       {"function", dist::token_type::tok_function},
      {"if", dist::token_type::tok_if},         {"null", dist::token_type::tok_null},
      {"or", dist::token_type::tok_or},         {"print", dist::token_type::tok_print},
      {"return", dist::token_type::tok_return}, {"super", dist::token_type::tok_super},
      {"this", dist::token_type::tok_this},     {"true", dist::token_type::tok_true},
      {"var", dist::token_type::tok_var},       {"while", dist::token_type::tok_while}};
  return reserved_words;
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
      auto comment_size = get_comment_size(code);
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
  case '"': {
    auto [valid_string_literal, str_size, newline_count] = get_str_literal_size(code);
    auto token_type = valid_string_literal ? dist::token_type::tok_string : dist::token_type::tok_invalid_token;
    return std::tuple(token_type, str_size, newline_count);
  }
  default:
    if (std::isdigit(code[0])) {
      auto num_size = get_number_literal_size(code);
      return std::tuple(dist::token_type::tok_number, num_size, 0);
    } else if (is_alpha(code[0])) {
      auto id_size = get_identifier_size(code);
      auto is_reserved_word = reserved_words().find(code.substr(0, id_size)) != reserved_words().end();
      auto token_type =
          is_reserved_word ? reserved_words().at(code.substr(0, id_size)) : dist::token_type::tok_identifier;
      return std::tuple(token_type, id_size, 0);
    } else {
      return std::tuple(dist::token_type::tok_invalid_token, 0, 0);
    }
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
    if (token_type != dist::token_type::tok_space && token_type != dist::token_type::tok_comment) {
      tokens.emplace_back(token);
    }
    if (token_type == dist::token_type::tok_invalid_token) {
      return tokens;
    }
    curr_pos_in_line += token_size;
    if (newline_count) {
      curr_pos_in_line = 0;
      curr_line += newline_count;
    }
    code = step(code, token_size);
  }
  tokens.emplace_back(dist::token(dist::token_type::tok_eof, "", curr_pos_in_line, curr_line));
  return tokens;
}
} // namespace dist
