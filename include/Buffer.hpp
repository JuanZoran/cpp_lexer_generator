#pragma once
#include <FSA.hpp>
#include <Token.hpp>
#include <cassert>
#include <istream>
#include <vector>

class Buffer {
public:
    using line_t = uint32_t;
    using column_t = uint32_t;

    using pos_t = int32_t;

public:
    Buffer() = delete;
    Buffer& operator= (Buffer&&) = delete;
    Buffer& operator= (const Buffer&) = delete;

    explicit Buffer(std::istream& istream, std::size_t BUFFER_SIZE = 1024):
        _istream(istream),
        _buffer(BUFFER_SIZE),
        _pos(0),
        _lexeme {},
        _lexeme_start(-1)
    {
        _refill();
    }

    ~Buffer() = default;
    Buffer(Buffer&&) = default;
    Buffer(const Buffer&) = default;

public:
    FSA::char_t peek() const;
    FSA::char_t take();
    void next();
    FSA::char_t get(pos_t pos) const;

    void markLexemeStart();
    Lexeme takeLexeme();

private:
    void _refill();


public:
    static constexpr pos_t COLUMN_START = 1;
    static constexpr FSA::char_t EOF_CHAR = -1;

private:
    std::istream& _istream;
    std::vector<FSA::char_t> _buffer;


    pos_t _pos;

    pos_t _lexeme_start;
    Lexeme _lexeme;
};

inline FSA::char_t Buffer::peek() const
{
    return _buffer[_pos];
}

inline FSA::char_t Buffer::take()
{
    auto c = peek();
    next();
    return c;
}

inline FSA::char_t Buffer::get(pos_t pos) const
{
    assert(pos >= 0 && pos < _buffer.size());
    return _buffer[pos];
}

inline void Buffer::markLexemeStart()
{
    _lexeme_start = _pos;
}

inline Lexeme Buffer::takeLexeme()
{
    assert(_lexeme_start != -1);

    _lexeme.value = std::string(_buffer.begin() + _lexeme_start, _buffer.begin() + _pos);
    _lexeme_start = -1;
    return _lexeme;
}

inline void Buffer::_refill()
{
    // check whether there is still lexeme in the buffer
    assert(_lexeme_start <= _pos);

    if (_lexeme_start != -1) {
        std::copy(_buffer.begin() + _lexeme_start, _buffer.begin() + _pos, _buffer.begin());
        _pos -= _lexeme_start;
        _lexeme_start = 0;
    }
    else {
        _pos = 0;
    }

    // read from istream
    assert(_buffer.size() > _pos);
    auto size = (_buffer.size() - _pos) * sizeof(FSA::char_t);
    _istream.read(static_cast<char*>(_buffer.data()) + _pos, size);

    auto bytes_read = _istream.gcount();
    if (bytes_read < size) {
        auto new_size = (_pos + bytes_read) / sizeof(FSA::char_t) + 1;
        _buffer[new_size - 1] = EOF_CHAR;
        _buffer.resize(new_size);
    }
}

inline void Buffer::next()
{
    ++_pos;
    if (_pos == _buffer.size()) {
        _refill();
    }
}
