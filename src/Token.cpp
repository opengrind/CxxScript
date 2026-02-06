#include "Token.h"
#include <sstream>

namespace Script {

std::string Token::toString() const {
  std::stringstream ss;
  ss << "Token(" << static_cast<int>(type) << ", '" << lexeme
     << "', line=" << line << ", col=" << column << ")";
  return ss.str();
}

} // namespace Script
