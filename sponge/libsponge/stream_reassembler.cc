#include "stream_reassembler.hh"

// Dummy implementation of a stream reassembler.

// For Lab 1, please replace with a real implementation that passes the
// automated checks run by `make check_lab1`.

// You will need to add private members to the class declaration in `stream_reassembler.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

StreamReassembler::StreamReassembler(const size_t capacity)
    : _output(capacity), _capacity(capacity), neededIndex(0), bytesUnassembled(0), tmpMap() {}
//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {
    // The StreamReassembler's job is: push ordered, unoverlapping substring byte by byte into byteStream.
    // and we have two memory space to maintain when coding: byteStream, tmpMap;

    if (index < neededIndex) {
        // some portion of substring is useful that can be stored in tmpMap
        int n = data.size();
        if (index + n > neededIndex) {
            for (int i = neededIndex - index; i < n; i++) {
                if (tmpMap.count(index + i) != 0) {
                    continue;
                }
                if (i != n - 1) {
                    tmpMap[index + i] = make_pair(data[i], false);
                } else {
                    tmpMap[index + i] = make_pair(data[i], eof);
                }
                bytesUnassembled++;
            }
        } else {
            return;
        }
    }
    // store in tmpMap
    if (index > neededIndex) {
        int n = data.size();
        for (int i = 0; i < n; i++) {
            if (tmpMap.count(index + i) != 0) {
                continue;
            }
            if (i != n - 1) {
                tmpMap[index + i] = make_pair(data[i], false);
            } else {
                tmpMap[index + i] = make_pair(data[i], eof);
            }
            bytesUnassembled++;
        }
        return;
    }
    // push into byteStream
    if (index == neededIndex) {
        int minSize = min(data.size(), _output.remaining_capacity());
        for (int i = 0; i < minSize; i++) {
            _output.write(data.substr(i, 1));
            neededIndex++;

            if (tmpMap.count(index + i) != 0) {
                tmpMap.erase(index + i);
                bytesUnassembled--;
            }
        }
        if (eof) {
            _output.end_input();
        }
    }

    while (tmpMap.count(neededIndex) != 0) {
        pair<string, bool> pair = tmpMap[neededIndex];
        if (_output.remaining_capacity()) {
            _output.write(pair.first);
            bool flag = pair.second;
            if (flag) {
                _output.end_input();
            }

            tmpMap.erase(neededIndex);
            bytesUnassembled--;
            neededIndex++;
        } else {
            break;
        }
    }
}

size_t StreamReassembler::unassembled_bytes() const {
    // the number of bytes stored in tmpMap
    return bytesUnassembled;
}

bool StreamReassembler::empty() const { return _output.eof(); }
