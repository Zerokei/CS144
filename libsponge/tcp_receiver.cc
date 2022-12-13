#include "tcp_receiver.hh"

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

void TCPReceiver::segment_received(const TCPSegment &seg) {
    // 设置 ISN
    if (seg.header().syn) {
        _ISN = seg.header().seqno;
    }
    if (!_ISN.has_value()) { // 如果还没有收到 syn，就舍弃该包
        return;
    }
    _checkpoint = unwrap(seg.header().seqno, _ISN.value(), _checkpoint);
    // Push any data, or end-of-stream marker, to the StreamReassembler
    if (seg.header().syn){ // 此时的 _checkpoint 是 syn 帧的地址
        _reassembler.push_substring(seg.payload().copy(), _checkpoint, seg.header().fin);
    }
    else {
        _reassembler.push_substring(seg.payload().copy(), _checkpoint - 1, seg.header().fin);
    }
    _ackno = wrap(_reassembler.stream_out().bytes_written(), _ISN.value()) + 1; // ackno 指向第一个没有接收的 idx
    if (_reassembler.stream_out().input_ended()) { // FIN_RECV
        _ackno = _ackno.value() + 1; 
    }
}

optional<WrappingInt32> TCPReceiver::ackno() const {
    if (_ISN.has_value()) {
        return _ackno;
    } else {
        return {}; 
    }
}

size_t TCPReceiver::window_size() const {
    if (!ackno().has_value()) return _capacity;
    return _capacity - _reassembler.stream_out().buffer_size();
}
