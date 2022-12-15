#include "tcp_receiver.hh"
#include <cstdint>

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

void TCPReceiver::segment_received(const TCPSegment &seg) {
    // 设置 ISN
    if (seg.header().syn) { // LISTEN -> SYN RECV
        _ISN = seg.header().seqno;
    }
    else if (not ackno().has_value()) { // LISTEN -> abort packge without syn
        return;
    }
    _checkpoint = unwrap(seg.header().seqno, _ISN.value(), _checkpoint);
    
    uint64_t index = seg.header().syn ? _checkpoint : _checkpoint - 1;
    
    // Push any data, or end-of-stream marker, to the StreamReassembler
    /*fin = true: SYN_RECV -> FIN_RECV
            false: nothing*/ 
    _reassembler.push_substring(seg.payload().copy(), index, seg.header().fin);

    _ackno = wrap(_reassembler.stream_out().bytes_written(), _ISN.value()) 
                + 1                                        // syn
                + _reassembler.stream_out().input_ended(); // fin
    // printf("ackno = %u\n", _ackno.value().raw_value());
}

optional<WrappingInt32> TCPReceiver::ackno() const {
    if (_ISN.has_value()) {
        return _ackno;
    } else {
        return {}; 
    }
}

size_t TCPReceiver::window_size() const {
    if (not ackno().has_value()) return _capacity;
    return _capacity - _reassembler.stream_out().buffer_size();
}
