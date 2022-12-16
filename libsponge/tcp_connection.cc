#include "tcp_connection.hh"
#include "tcp_state.hh"

#include <cstdint>
#include <iostream>
#include <cassert>

// Dummy implementation of a TCP connection

// For Lab 4, please replace with a real implementation that passes the
// automated checks run by `make check`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

size_t TCPConnection::remaining_outbound_capacity() const { return _sender.stream_in().remaining_capacity(); }

size_t TCPConnection::bytes_in_flight() const { return _sender.bytes_in_flight(); }

size_t TCPConnection::unassembled_bytes() const { return _receiver.unassembled_bytes(); }

size_t TCPConnection::time_since_last_segment_received() const { return _ms_since_last_segment_received; }

size_t TCPConnection::write(const string &data) { 
    size_t write_data_size =  _sender.write_data(data); 
    send_segments();
    return write_data_size;
}

void TCPConnection::send_empty_segment() { _sender.send_empty_segment(); send_segments(); }

void TCPConnection::send_rst_segment(){ _sender.send_RST_segment(); send_segments(); }

bool TCPConnection::active() const { return _is_active; }

void TCPConnection::shutdown_connection() { _is_active = false; }

void TCPConnection::reset() {
    _sender.stream_in().set_error();
    _receiver.stream_out().set_error();
    _linger_after_streams_finish = false;
    shutdown_connection();
}

void TCPConnection::wait_enough_time() {
    if (is_state(TCPState::State::TIME_WAIT)) {
        _linger_after_streams_finish = false;
        shutdown_connection();
    }
}

void TCPConnection::segment_received(const TCPSegment &seg) { 
    bool prev_closing = is_state(TCPState::State::CLOSING);
    bool prev_fin_wait_1 = is_state(TCPState::State::FIN_WAIT_1);
    bool prev_last_ack = is_state(TCPState::State::LAST_ACK);

    _ms_since_last_segment_received = 0; 
    if (seg.header().rst) { // RST packge
        reset();
        return;
    }
    if (is_state(TCPState::State::LISTEN) and not seg.header().syn){
        return;
    } 
    _receiver.segment_received(seg); 

    if (TCPState::state_summary(_receiver) == TCPReceiverStateSummary::FIN_RECV and
        TCPState::state_summary(_sender) == TCPSenderStateSummary::SYN_ACKED){
        _linger_after_streams_finish = false;
        assert(is_state(TCPState::State::CLOSE_WAIT));
    }

    // UPDATE SENDER
    _sender.ack_received(seg.header().ackno, seg.header().win);
    _sender.fill_window();

    // PASIVE CLOSE
    if (prev_last_ack and seg.header().ack and seg.header().ackno.raw_value() == _sender.next_seqno().raw_value()) {
        // printf("CLOSED!");
        shutdown_connection();
        return;
    }

    // ACTIVE CLOSE
    if (prev_closing and is_state(TCPState::State::TIME_WAIT)) {
        return;
    }
    if (prev_fin_wait_1 and is_state(TCPState::State::FIN_WAIT_2)) {
        return;
    }

    // SEND THE SEGMENTS
    if (_sender.segments_out().empty()) {
        if (_receiver.ackno().has_value() and seg.length_in_sequence_space() == 0 and seg.header().seqno == _receiver.ackno().value() - 1) {
            send_empty_segment();
        } else if (seg.length_in_sequence_space() > 0) {
            send_empty_segment();
        }
    }
    else send_segments();

}


void TCPConnection::send_segments() {
    while(not _sender.segments_out().empty()) {
        // POP from _sender
        TCPSegment tcp_segment = _sender.segments_out().front();
        _sender.segments_out().pop();

        // SET ack, ackno, win
        if (_receiver.ackno().has_value() && not tcp_segment.header().rst) {
            tcp_segment.header().ack = true;
            tcp_segment.header().ackno = _receiver.ackno().value();
        }
        tcp_segment.header().win = static_cast<uint16_t>(min(static_cast<size_t>(numeric_limits<uint16_t>::max()) ,_receiver.window_size()));
        // PUSH to the output queue 
        _segments_out.push(tcp_segment);
    }
}


//! \param[in] ms_since_last_tick number of milliseconds since the last call to this method
void TCPConnection::tick(const size_t ms_since_last_tick) { 
    _ms_since_last_segment_received += ms_since_last_tick;
    _sender.tick(ms_since_last_tick);
    send_segments();
    if (time_since_last_segment_received() >= 10 * static_cast<uint64_t>(_cfg.rt_timeout)) {
        wait_enough_time();
    }
    if (_sender.consecutive_retransmissions() > TCPConfig::MAX_RETX_ATTEMPTS) {
        // SEND RST segment
        send_rst_segment();
        // ABORT the connection  
        reset();
    }
}

void TCPConnection::end_input_stream() {
    _sender.stream_in().end_input();
    _sender.fill_window();
    send_segments();
}

void TCPConnection::connect() {
    _sender.fill_window();
    send_segments();
}
TCPConnection::~TCPConnection() {
    try {
        if (active()) {
            cerr << "Warning: Unclean shutdown of TCPConnection\n";
            send_rst_segment();
        }
    } catch (const exception &e) {
        std::cerr << "Exception destructing TCP FSM: " << e.what() << std::endl;
    }
}
