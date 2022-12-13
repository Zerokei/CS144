#include "network_interface.hh"

#include "arp_message.hh"
#include "ethernet_frame.hh"

#include <iostream>

// Dummy implementation of a network interface
// Translates from {IP datagram, next hop address} to link-layer frame, and from link-layer frame to IP datagram

// For Lab 5, please replace with a real implementation that passes the
// automated checks run by `make check_lab5`.

using namespace std;

//! \param[in] ethernet_address Ethernet (what ARP calls "hardware") address of the interface
//! \param[in] ip_address IP (what ARP calls "protocol") address of the interface
NetworkInterface::NetworkInterface(const EthernetAddress &ethernet_address, const Address &ip_address)
    : _ethernet_address(ethernet_address), _ip_address(ip_address) {
    cerr << "DEBUG: Network interface has Ethernet address " << to_string(_ethernet_address) << " and IP address "
         << ip_address.ip() << "\n";
}

//! \param[in] dgram the IPv4 datagram to be sent
//! \param[in] next_hop the IP address of the interface to send it to (typically a router or default gateway, but may also be another host if directly connected to the same network as the destination)
//! (Note: the Address type can be converted to a uint32_t (raw 32-bit IP address) with the Address::ipv4_numeric() method.)
void NetworkInterface::send_datagram(const InternetDatagram &dgram, const Address &next_hop) {
    // convert IP address of next hop to raw 32-bit representation (used in ARP header)

    const uint32_t next_hop_ip = next_hop.ipv4_numeric();
    auto dst = arp_map.find(next_hop_ip);
    if (dst != arp_map.end()){
        EthernetHeader _ethernet_header;
        _ethernet_header.dst = (dst->second)._ethernet_address;
        _ethernet_header.src = _ethernet_address;
        _ethernet_header.type = EthernetHeader::TYPE_IPv4;
        EthernetFrame _ethernet_frame;
        _ethernet_frame.header() = _ethernet_header;
        _ethernet_frame.payload() = dgram.serialize();
        _frames_out.push(_ethernet_frame);
    }else {
        _frames_wait.push_back(std::make_pair(dgram, next_hop));
        if (arp_request.find(next_hop_ip) != arp_request.end()) { // is querying
            return;
        } else { // query
            EthernetHeader _ethernet_header;
            _ethernet_header.dst = ETHERNET_BROADCAST;
            _ethernet_header.src = _ethernet_address;
            _ethernet_header.type = EthernetHeader::TYPE_ARP;

            ARPMessage arp_req;
            arp_req.opcode = ARPMessage::OPCODE_REQUEST;
            arp_req.sender_ethernet_address = _ethernet_address;
            arp_req.sender_ip_address = _ip_address.ipv4_numeric();
            arp_req.target_ip_address = next_hop_ip;

            EthernetFrame _ethernet_frame;
            _ethernet_frame.header() = _ethernet_header;
            _ethernet_frame.payload() = arp_req.serialize();
            _frames_out.push(_ethernet_frame);
            // update arp_request
            arp_request.insert(std::pair<uint32_t, size_t>(next_hop_ip, 5000)); // expire in 5s
        }
    }
}

#define DEBUG_INFO cerr << "parse Ethernet Frame Wrong!" << endl;

//! \param[in] frame the incoming Ethernet frame
optional<InternetDatagram> NetworkInterface::recv_frame(const EthernetFrame &frame) {
    if(frame.header().type == EthernetHeader::TYPE_IPv4) {
        if(frame.header().dst != _ethernet_address) return {};
        InternetDatagram dgram;
        if(dgram.parse(frame.payload()) == ParseResult::NoError) {
            return dgram;
        } else {
            DEBUG_INFO
        }
    } else if (frame.header().type == EthernetHeader::TYPE_ARP) {
        ARPMessage arp;
        if(arp.parse(frame.payload()) == ParseResult::NoError) {
            if(arp.target_ip_address != _ip_address.ipv4_numeric()) return {};

            // update arp request
            auto it = arp_request.find(arp.sender_ip_address);
            if(it != arp_request.end()) arp_request.erase(it);
            // set up the arp map
            if (arp_map.find(arp.sender_ip_address) == arp_map.end())
                arp_map.insert(std::pair<uint32_t, arp_info>(
                    arp.sender_ip_address, arp_info(arp.sender_ethernet_address)
                ));


            // send the datagram
            auto frame_it = _frames_wait.begin();
            while (frame_it != _frames_wait.end()) {
                if ((frame_it->second).ipv4_numeric() == arp.sender_ip_address) {
                    send_datagram(frame_it->first, frame_it->second);
                    frame_it = _frames_wait.erase(frame_it);
                } else {
                    ++frame_it;
                }
            }
            if (arp.opcode == ARPMessage::OPCODE_REQUEST) {
                EthernetHeader _ethernet_header;
                _ethernet_header.dst    = arp.sender_ethernet_address;
                _ethernet_header.src    = _ethernet_address;
                _ethernet_header.type   = EthernetHeader::TYPE_ARP;

                ARPMessage arp_req;
                arp_req.opcode                  = ARPMessage::OPCODE_REPLY;
                arp_req.sender_ip_address       = _ip_address.ipv4_numeric();
                arp_req.sender_ethernet_address = _ethernet_address;
                arp_req.target_ip_address       = arp.sender_ip_address;
                arp_req.target_ethernet_address = arp.sender_ethernet_address;

                EthernetFrame _ethernet_frame;
                _ethernet_frame.header() = _ethernet_header;
                _ethernet_frame.payload() = arp_req.serialize();
                _frames_out.push(_ethernet_frame);
            }         
        } else {
            DEBUG_INFO
        }
    }
    return {};
}

//! \param[in] ms_since_last_tick the number of milliseconds since the last call to this method
void NetworkInterface::tick(const size_t ms_since_last_tick) {
    { // update the ARP Map
        auto it = arp_map.begin();
        while (it != arp_map.end()) {

            (it->second).ttl -= ms_since_last_tick;
            if ((it->second).ttl > 30000) {
                it = arp_map.erase(it); // c++11, point to the next one of erase item
            } else {
                ++it; // next
            }
        }
    }
    { // update the ARP Query
        auto it = arp_request.begin();
        while (it != arp_request.end()) {
            it->second -= ms_since_last_tick;
            if (it->second > 5000) {
                it->second = 5000; // reset time

                EthernetHeader _ethernet_header;
                _ethernet_header.dst = ETHERNET_BROADCAST;
                _ethernet_header.src = _ethernet_address;
                _ethernet_header.type = EthernetHeader::TYPE_ARP;

                ARPMessage arp_req;
                arp_req.opcode = ARPMessage::OPCODE_REQUEST;
                arp_req.sender_ethernet_address = _ethernet_address;
                arp_req.sender_ip_address = _ip_address.ipv4_numeric();
                arp_req.target_ip_address = it->first;

                EthernetFrame _ethernet_frame;
                _ethernet_frame.header() = _ethernet_header;
                _ethernet_frame.payload() = arp_req.serialize();
                _frames_out.push(_ethernet_frame);
            }
            ++it; // next
        }
    }
}