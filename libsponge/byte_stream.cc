#include "byte_stream.hh"

// Dummy implementation of a flow-controlled in-memory byte stream.

// For Lab 0, please replace with a real implementation that passes the
// automated checks run by `make check_lab0`.

// You will need to add private members to the class declaration in `byte_stream.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

ByteStream::ByteStream(const size_t _capacity): capacity(_capacity),
                                                read_len(0),
                                                write_len(0),
                                                str(""),
                                                end_sig(false){}

size_t ByteStream::write(const string &data) {
    if (str.length() + data.length() <= capacity) {
        //printf("str.len = %lu, data.len = %lu\n", str.length(), data.length());
        str = str + data;
        write_len += data.length();
        return data.length();
    } else {
        size_t input_len = capacity - str.length();
        str = str + data.substr(0, input_len);
        write_len += input_len;
        return input_len;
    }
}

//! \param[in] len bytes will be copied from the output side of the buffer
string ByteStream::peek_output(const size_t len) const {
    if (len > str.length()) {
        return ""; // request length is too big
    }
    else return str.substr(0, len);
}

//! \param[in] len bytes will be removed from the output side of the buffer
void ByteStream::pop_output(const size_t len) {
    if (len > str.length()) {
        return; // request length is too big
    }
    else {
        read_len += len;
        str = str.substr(len, str.length() - len);
        return;
    }
}

//! Read (i.e., copy and then pop) the next "len" bytes of the stream
//! \param[in] len bytes will be popped and returned
//! \returns a string
std::string ByteStream::read(const size_t len) {
    string read_string = peek_output(len);
    pop_output(len);
    return read_string;
}

void ByteStream::end_input() { end_sig = true;}

bool ByteStream::input_ended() const { return end_sig; }

size_t ByteStream::buffer_size() const { return str.length(); }

bool ByteStream::buffer_empty() const { return str.empty(); }

bool ByteStream::eof() const { return buffer_size() == 0 && input_ended(); }

size_t ByteStream::bytes_written() const { return write_len; }

size_t ByteStream::bytes_read() const { return read_len; }

size_t ByteStream::remaining_capacity() const { return capacity - str.length(); }
