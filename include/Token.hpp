#pragma once
#include <string>

struct Lexeme
{
    std::string value;
};

struct Token
{
    std::string type;
    std::string value;
};
