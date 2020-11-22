#ifndef DIST_LEXING_H
#define DIST_LEXING_H

#include <dist/token.hpp>
#include <vector>

namespace dist {
std::vector<dist::token> scan_tokens(std::string_view code);
} // namespace dist

#endif