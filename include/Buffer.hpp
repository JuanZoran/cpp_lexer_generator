#pragma once
#include <FSA.hpp>
#include <cassert>
#include <iostream>
#include <vector>

#if 1
class Buffer {
public:
    using linenr_t = int32_t;
    using column_t = int32_t;
    constexpr static linenr_t INVALID_LINENR = -1;
    constexpr static column_t INVALID_COLUMN = -1;
    constexpr static char EOF_CHAR = -1;

public:
    Buffer(std::istream& istream);

    ~Buffer() = default;
    Buffer(Buffer&&) = default;
    Buffer(const Buffer&) = default;
    Buffer& operator= (Buffer&&) = default;
    Buffer& operator= (const Buffer&) = default;

public:
    void printLines() const;
    char peek() const;
    char take();
    void next();
    void rollback();
    void rollback(column_t column);
    void rollback(linenr_t linenr, column_t column);

    linenr_t getLineNr() const;
    column_t getColumn() const;
    void markLexemeStart();
    std::string takeLexeme();

private:
    std::vector<std::string> _buffer {};
    linenr_t _cur_linenr {};
    column_t _cur_column {};

    linenr_t _lexeme_start_linenr {};
    column_t _lexeme_start_column {};
};

inline Buffer::Buffer(std::istream& istream):
    _lexeme_start_linenr { INVALID_LINENR },
    _lexeme_start_column { INVALID_COLUMN }
{
    for (std::string line; std::getline(istream, line);) {
        _buffer.push_back(std::move(line));
    }
}

inline void Buffer::printLines() const
{
    for (auto& line : _buffer) {
        std::cout << line << std::endl;
    }
}

inline char Buffer::peek() const
{
    if (_cur_linenr == _buffer.size())
        return EOF_CHAR;
    return _buffer[_cur_linenr][_cur_column];
}

inline char Buffer::take()
{
    auto c = peek();
    next();
    return c;
}

inline void Buffer::next()
{
    assert(_cur_linenr < _buffer.size());
    assert(_cur_column < _buffer[_cur_linenr].size());

    ++_cur_column;
    if (_cur_column == _buffer[_cur_linenr].size()) {
        ++_cur_linenr;
        _cur_column = 0;
    }
}

inline void Buffer::rollback()
{
    rollback(_cur_linenr, _cur_column - 1);
}

inline void Buffer::rollback(column_t column)
{
    rollback(_cur_linenr, column);
}

inline void Buffer::rollback(linenr_t linenr, column_t column)
{
    assert(linenr < _buffer.size());
    assert(column < _buffer[linenr].size());
    _cur_linenr = linenr;
    _cur_column = column;
}

inline void Buffer::markLexemeStart()
{
    _lexeme_start_linenr = _cur_linenr;
    _lexeme_start_column = _cur_column;
}

inline std::string Buffer::takeLexeme()
{
    assert(_lexeme_start_linenr != INVALID_LINENR);
    assert(_lexeme_start_column != INVALID_COLUMN);

    std::string lexeme;
    if (_lexeme_start_linenr == _cur_linenr) {
        lexeme = _buffer[_lexeme_start_linenr].substr(
            _lexeme_start_column, _cur_column - _lexeme_start_column);
    }
    else {
        lexeme = _buffer[_lexeme_start_linenr].substr(_lexeme_start_column);
        for (auto i = _lexeme_start_linenr + 1; i < _cur_linenr; ++i) {
            lexeme += _buffer[i];
        }
        lexeme += _buffer[_cur_linenr].substr(0, _cur_column);
    }


    _lexeme_start_linenr = INVALID_LINENR;
    _lexeme_start_column = INVALID_COLUMN;
    return lexeme;
}

inline Buffer::linenr_t Buffer::getLineNr() const
{
    return _cur_linenr;
}

inline Buffer::column_t Buffer::getColumn() const
{
    return _cur_column;
}

#else // Version 1
#include <FSA.hpp>
#include <Token.hpp>
#include <cassert>
#include <istream>
#include <vector>

// class Buffer {
// public:
//     using line_t = uint32_t;
//     using column_t = uint32_t;

//     using pos_t = int32_t;

// public:
//     Buffer() = delete;
//     Buffer& operator= (Buffer&&) = delete;
//     Buffer& operator= (const Buffer&) = delete;

//     explicit Buffer(std::istream& istream, std::size_t BUFFER_SIZE = 1024):
//         _istream(istream),
//         _buffer(BUFFER_SIZE),
//         _pos(0),
//         _lexeme {},
//         _lexeme_start(-1)
//     {
//         _refill();
//     }

//     ~Buffer() = default;
//     Buffer(Buffer&&) = default;
//     Buffer(const Buffer&) = default;

// public:
//     FSA::char_t peek() const;
//     FSA::char_t take();
//     void next();
//     FSA::char_t get(pos_t pos) const;

//     void markLexemeStart();
//     Lexeme takeLexeme();

// private:
//     void _refill();


// public:
//     static constexpr pos_t COLUMN_START = 1;
//     static constexpr FSA::char_t EOF_CHAR = -1;

// private:
//     std::istream& _istream;
//     std::vector<FSA::char_t> _buffer;


//     pos_t _pos;

//     pos_t _lexeme_start;
//     Lexeme _lexeme;
// };

// inline FSA::char_t Buffer::peek() const
// {
//     return _buffer[_pos];
// }

// inline FSA::char_t Buffer::take()
// {
//     auto c = peek();
//     next();
//     return c;
// }

// inline FSA::char_t Buffer::get(pos_t pos) const
// {
//     assert(pos >= 0 && pos < _buffer.size());
//     return _buffer[pos];
// }

// inline void Buffer::markLexemeStart()
// {
//     _lexeme_start = _pos;
// }

// inline Lexeme Buffer::takeLexeme()
// {
//     assert(_lexeme_start != -1);

//     _lexeme.value = std::string(_buffer.begin() + _lexeme_start, _buffer.begin() + _pos);
//     _lexeme_start = -1;
//     return _lexeme;
// }

// inline void Buffer::_refill()
// {
//     // check whether there is still lexeme in the buffer
//     assert(_lexeme_start <= _pos);

//     if (_lexeme_start != -1) {
//         std::copy(_buffer.begin() + _lexeme_start, _buffer.begin() + _pos, _buffer.begin());
//         _pos -= _lexeme_start;
//         _lexeme_start = 0;
//     }
//     else {
//         _pos = 0;
//     }

//     // read from istream
//     assert(_buffer.size() > _pos);
//     auto size = (_buffer.size() - _pos) * sizeof(FSA::char_t);
//     _istream.read(static_cast<char*>(_buffer.data()) + _pos, size);

//     auto bytes_read = _istream.gcount();
//     if (bytes_read < size) {
//         auto new_size = (_pos + bytes_read) / sizeof(FSA::char_t) + 1;
//         _buffer[new_size - 1] = EOF_CHAR;
//         _buffer.resize(new_size);
//     }
// }

// inline void Buffer::next()
// {
//     ++_pos;
//     if (_pos == _buffer.size()) {
//         _refill();
//     }
// }
#endif
