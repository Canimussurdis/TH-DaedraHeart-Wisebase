
#include "Token.h"
#include "../entity/User.h"

#include <Wt/Dbo/Impl.h>

DBO_INSTANTIATE_TEMPLATES(Token);

Token::Token()
{ }

Token::Token(const std::string& v, const Wt::WDateTime& e)
  : value(v),
    expires(e)
{ }
