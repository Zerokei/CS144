#include "tcp_sender.hh"

#include "tcp_config.hh"

#include <cstddef>
#include <random>

// Dummy implementation of a TCP sender

// For Lab 3, please replace with a real implementation that passes the
// automated checks run by `make check_lab3`.

#define UNWRAP(x) unwrap(x, _isn, _next_seqno)
#define LENGTH(x) ((x).length_in_sequence_space())

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

//! \param[in] capacity the capacity of the outgoing byte stream
//! \param[in] retx_timeout the initial amount of time to wait before retransmitting the oldest outstanding segment
//! \param[in] fixed_isn the Initial Sequence Number to use, if set (otherwise uses a random ISN)
TCPSender::TCPSender(const size_t capacity, const uint16_t retx_timeout, const std::optional<WrappingInt32> fixed_isn)
    : _isn(fixed_isn.value_or(WrappingInt32{random_device()()}))
    , _initial_retransmission_timeout{retx_timeout}
    , _stream(capacity) 
    , _RTO(retx_timeout) {}

uint64_t TCPSender::bytes_in_flight() const { 
    // 求未接收的字节数
    uint64_t total_size = 0;
    for (auto it = _segments_trace.begin(); it != _segments_trace.end(); ++it) {
        total_size += LENGTH(*it);
    }
    return total_size;
}
size_t TCPSender::write_data(const string &data) {
    size_t write_data_size = stream_in().write(data);
    fill_window(); // Wrap the new coming data
    return write_data_size;
}

void TCPSender::fill_window() {
    uint16_t window_size = _window_size > 0 ? _window_size : 1;

    while (window_size > bytes_in_flight()) {
        // 比较窗口大小 _window_size，待发送数据 _stream.buffer_size 
        // 和限制单次发送大小 TCPConfig::MAX_PAYLOAD_SIZE
        size_t send_size = min(TCPConfig::MAX_PAYLOAD_SIZE, _stream.buffer_size());
        if (send_size > window_size - bytes_in_flight()) {
            send_size = window_size - bytes_in_flight();
        }
        // 若未发送SYN数据包，则设置header的SYN位
        TCPSegment tcp_segment;
        if (next_seqno_absolute() == 0) { // CLOSED
            tcp_segment.header().syn = true;
        } 
        // 设置 seqno 和 payload
        tcp_segment.header().seqno = wrap(_next_seqno, _isn); // SET _next_seqno
        tcp_segment.payload() = Buffer(_stream.read(send_size));

        // 数据发送完毕且能发送FIN，且还未发送过FIN
        if (!_has_sent_fin && stream_in().eof() && send_size + 1 <= window_size - bytes_in_flight()) {
            tcp_segment.header().fin = true;
            _has_sent_fin = true;
        }
        // 如果没有任何数据，就不发送
        if (LENGTH(tcp_segment) == 0) {
            return;
        }
        // 如果没有正在等待的数据包，就重设更新时间
        if (_segments_trace.empty()) {
            _retransmission_timer = _RTO;
        }
        // 发送数据包并追踪
        _segments_out.push(tcp_segment);
        _segments_trace.push_back(tcp_segment);
        // 更新待发送的abs seqno
        _next_seqno += LENGTH(tcp_segment);

        if (_has_sent_fin) break;
    }
}

//! \param ackno The remote receiver's ackno (acknowledgment number)
//! \param window_size The remote receiver's advertised window size
void TCPSender::ack_received(const WrappingInt32 ackno, const uint16_t window_size) { 
    if (UNWRAP(ackno) > _next_seqno) return; // illegal

    auto it = _segments_trace.begin();
    while (it != _segments_trace.end()) {
        if (UNWRAP((*it).header().seqno) + LENGTH(*it) <= UNWRAP(ackno)) {
            it = _segments_trace.erase(it);
        } 
        else it++;
    }

    if (!_previous_ack.has_value() || (_previous_ack.has_value() && UNWRAP(_previous_ack.value()) < UNWRAP(ackno))) {
        _previous_ack = ackno;
        _RTO = _initial_retransmission_timeout;
        if (!_segments_trace.empty()) _retransmission_timer = _RTO; // reset the timer
        else _retransmission_timer = {}; // stop the timer
        _consecutive_retransmissions = 0;
    }

    _window_size = window_size; 
}

//! \param[in] ms_since_last_tick the number of milliseconds since the last call to this method
void TCPSender::tick(const size_t ms_since_last_tick) {
    if (!_retransmission_timer.has_value()) { // 没有开启计时器
        return;
    }
    if (_retransmission_timer.value() > ms_since_last_tick) {
        _retransmission_timer.value() -= ms_since_last_tick;
        return;
    }

    auto it = _segments_trace.begin();
    optional<TCPSegment> resend_segment = {};
    while (it != _segments_trace.end()) {
        if (!resend_segment.has_value()) resend_segment = *it;
        else if (UNWRAP((it->header()).seqno) < UNWRAP(resend_segment.value().header().seqno)) {
            resend_segment = *it; 
        }
        ++it;
    }
    if (!resend_segment.has_value()) {
        return;
    }
    // 如果未接收队列非空
    else {
        // 重新发送数据段
        // 如果窗口大小非空
        if (_window_size > 0) {
            _consecutive_retransmissions++;
            _RTO = _RTO * 2;
        }
        if (_consecutive_retransmissions > TCPConfig::MAX_RETX_ATTEMPTS) {
            return;
        }
        _segments_out.push(resend_segment.value());
        // 重新设置计时器
        _retransmission_timer = _RTO;
    } 
}

unsigned int TCPSender::consecutive_retransmissions() const {
    return _consecutive_retransmissions;
}

void TCPSender::send_empty_segment() {
    TCPSegment tcp_segment;
    tcp_segment.header().seqno = wrap(_next_seqno, _isn);
    _segments_out.push(tcp_segment);
}

void TCPSender::send_RST_segment() {
    TCPSegment tcp_segment;
    tcp_segment.header().seqno = wrap(_next_seqno, _isn);
    tcp_segment.header().rst = true;
    _segments_out.push(tcp_segment);
}
