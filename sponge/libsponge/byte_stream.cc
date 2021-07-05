#include "byte_stream.hh"

#include <algorithm>
// Dummy implementation of a flow-controlled in-memory byte stream.

// For Lab 0, please replace with a real implementation that passes the
// automated checks run by `make check_lab0`.

// You will need to add private members to the class declaration in `byte_stream.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

ByteStream::ByteStream(const size_t capacity)
    : _capacity(capacity)
    , bytesWriteTotal(0)
    , bytesReadTotal(0)
    , _end_input(false)
    , stream()
    , _error(false) {}

size_t ByteStream::write(const string &data) {
    size_t i;
    size_t n = min(data.size(), remaining_capacity());
    for (i = 0; i < n; i++) {
        stream.push_back(data[i]);
    }
    bytesWriteTotal += n;
    return n;
}

//! \param[in] len bytes will be copied from the output side of the buffer
string ByteStream::peek_output(const size_t len) const {
    size_t n = min(len, buffer_size());
    return string(stream.begin(), stream.begin() + n);
}

//! \param[in] len bytes will be removed from the output side of the buffer
void ByteStream::pop_output(const size_t len) {
    size_t i;
    size_t n = min(len, buffer_size());
    for (i = 0; i < n; i++) {
        stream.pop_front();
    }
    bytesReadTotal += n;
}

//! Read (i.e., copy and then pop) the next "len" bytes of the stream
//! \param[in] len bytes will be popped and returned
//! \returns a string
std::string ByteStream::read(const size_t len) {
    string str = peek_output(len);
    pop_output(len);
    return str;
}

void ByteStream::end_input() { _end_input = true; }

bool ByteStream::input_ended() const { return _end_input; }

size_t ByteStream::buffer_size() const { return stream.size(); }

bool ByteStream::buffer_empty() const { return (buffer_size() == 0); }

bool ByteStream::eof() const {
    if (buffer_empty() && input_ended()) {
        return true;
    }
    return false;
}

size_t ByteStream::bytes_written() const { return bytesWriteTotal; }

size_t ByteStream::bytes_read() const { return bytesReadTotal; }

size_t ByteStream::remaining_capacity() const { return (_capacity - buffer_size()); }
