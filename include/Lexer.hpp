#pragma once
#include <Buffer.hpp>
#include <DFA.hpp>
#include <Token.hpp>
#include <color.h>
#include <fmt/format.h>

// TODO : Add filename, line, column support
class Lexer {
public:

public:
    Lexer() = delete;
    Lexer& operator= (Lexer&&) = delete;
    Lexer& operator= (const Lexer&) = delete;

    ~Lexer() = default;
    Lexer(Lexer&&) = default;
    Lexer(const Lexer&) = default;

    Lexer(Buffer buf, const DFA& dfa);

public:
    std::optional<Token> nextToken();
    std::vector<Token> getAllTokens();

private:
    const DFA _dfa;
    Buffer _buffer;


    DFA::state_t _current_state {};
    // 4 padding
    int32_t _padding {};
};

inline Lexer::Lexer(Buffer buf, const DFA& dfa):
    _dfa(dfa),
    _buffer(buf)
{
}

inline std::optional<Token> Lexer::nextToken()
{
    _current_state = _dfa.getStartState();
    _buffer.markLexemeStart();
    DFA::state_t last_final_state = DFA::INVALID_STATE;

    while (true) {
        const auto ch = _buffer.peek();
        if (ch == Buffer::EOF_CHAR)
            break;

        const auto reached_state = _dfa.getReachedState(_current_state, ch);
        if (!reached_state) {
            if (last_final_state == DFA::INVALID_STATE) {
                std::cout << Color::Red
                          << fmt::format(
                                 "Lexer error: Unexpected character {} at line [{}], column [{}]",
                                 ch,
                                 _buffer.getLineNr(),
                                 _buffer.getColumn())
                          << Color::Endl;
                exit(1);
            }
            break;
        }

        _current_state = *reached_state;
        _buffer.next();
        if (_dfa.isFinalState(_current_state))
            last_final_state = _current_state;
    }

    // clang-format off
    return last_final_state == DFA::INVALID_STATE
             ? std::nullopt
             : std::make_optional<Token>(Token {
                 .type = _dfa.getStateInfo(last_final_state),
                 .value = _buffer.takeLexeme(),
             });
    // clang-format on
}
