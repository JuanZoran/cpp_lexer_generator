#pragma once
#include <iostream>
#include <string>

struct Lexeme
{
    std::string value;
};

struct Token
{
    std::string type;
    std::string value;

    void print()
    {
        std::cout << "type: " << type << " | "
                  << "value: " << value << std::endl;
    }
};
