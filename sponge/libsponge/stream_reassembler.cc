#include "stream_reassembler.hh"

// Dummy implementation of a stream reassembler.

// For Lab 1, please replace with a real implementation that passes the
// automated checks run by `make check_lab1`.

// You will need to add private members to the class declaration in `stream_reassembler.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

StreamReassembler::StreamReassembler(const size_t capacity) : _output(capacity), _capacity(capacity), neededIndex(0), bytesUnassembled(0), tmpMap() {}

// push len size data that start with index into byteStream and store the other portion of data in tmpMap
void StreamReassembler::push_data(const string &data, const size_t index, const size_t len, const bool eof) {
	_output.write(data.substr(0, len));
	neededIndex += len;		
	// looking for in tmpMap to search whether there is a substring start with index
	auto it = tmpMap.find(index);
	if (it != tmpMap.end()) {
		string str = it->second.first;	
		if (str.size() > len) {
			str = data.size() > str.size() ? data : str;
			string newStr = str.substr(len, -1);
			bool flag = it->second.second;
			int newIndex = index + len;	
			auto newit = tmpMap.find(newIndex);	 
			if (newit == tmpMap.end()) {
				tmpMap[newIndex] = make_pair(newStr, flag);
				bytesUnassembled += newStr.size();
			}
			if (newit != tmpMap.end() && newit->second.first < newStr) {
				tmpMap[newIndex] = make_pair(newStr, flag);
				bytesUnassembled += (newStr.size() - newit->second.first.size());
			}
				
		}	
		tmpMap.erase(index);
	} else {
		if (len < data.size()) {
			string newStr = data.substr(len, -1);	
			bool flag = eof;
			int newIndex = index + len;	
			auto newit = tmpMap.find(newIndex);	 
			if (newit == tmpMap.end()) {
				tmpMap[newIndex] = make_pair(newStr, flag);
				bytesUnassembled += newStr.size();
			}
			if (newit != tmpMap.end() && newit->second.first < newStr) {
				tmpMap[newIndex] = make_pair(newStr, flag);
				bytesUnassembled += (newStr.size() - newit->second.first.size());
			}

		}	
	}	
	if (eof) {
		_output.end_input();	
	}

}
//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {
	// For substring "data" in the entire sequence, there are three cases:
	// 1. "data" is current needed substring.
	// 2. "data" is an overlapping substring, maybe some portion overlapping, maybe entire substring overlapping.
	// 3. "data" is an unordered substring.	
	// The StreamReassembler's job is: push ordered, unoverlapping substring into byteStream.
	size_t len = min(data.size(), _output.remaining_capacity());
	// data is current needed substring
	if (index == neededIndex) {
		push_data(data, index, len, eof);
	}
	// data is an overlapping substring
	if (index < neededIndex) {
		// some portion of substring is useful
		if (index + len > neededIndex) {
			string tmpData = data.substr(neededIndex-index, -1);
			push_data(tmpData, neededIndex, min(tmpData.size(), len), eof);		 
		}	
	} 
	// data is an unordered substring, store it in tmpMap.
	if (index > neededIndex) {
		auto it = tmpMap.find(index);	
		if (it == tmpMap.end()) {
			tmpMap[index] = make_pair(data, eof);
			bytesUnassembled += data.size();
		}
		if (it != tmpMap.end() && it->second.first < data) {
			tmpMap[index] = make_pair(data, eof);
			bytesUnassembled += (data.size() - it->second.first.size());
		}
	}
	// search in tmpMap to see whether there is a substring start with index
	auto it = tmpMap.find(index);
	if (it != tmpMap.end()) {
		string tmpData = it->second.first;
		bool flag = it->second.second;
		// push into bytestream	
		len = min(tmpData.size(), _output.remaining_capacity());	
		_output.write(data.substr(0, len));
		neededIndex += len;	

		if (tmpData.size() > len) {
			string newStr = tmpData.substr(len, -1);	
			int newIndex = index + len;	
			auto newit = tmpMap.find(newIndex);	 
			if (newit == tmpMap.end()) {
				tmpMap[newIndex] = make_pair(newStr, flag);
				bytesUnassembled += newStr.size();
			}
			if (newit != tmpMap.end() && newit->second.first < newStr) {
				tmpMap[newIndex] = make_pair(newStr, flag);
				bytesUnassembled += (newStr.size() - newit->second.first.size());
			}

		}
	
	}
	
}

size_t StreamReassembler::unassembled_bytes() const { 
	// the number of bytes stored in tmpMap	
	return bytesUnassembled; 
}

bool StreamReassembler::empty() const { 
    return _output.eof(); 
}
