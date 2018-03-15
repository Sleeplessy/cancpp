//
// Created by sleeplessy on 2/24/18.
//

#ifndef EMU_CS_BASIC_CAN_INTERFACE_HPP
#define EMU_CS_BASIC_CAN_INTERFACE_HPP

#include <string>
#include <linux/can.h>
#include <stdexcept>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>
#include <deque>

struct socket_interface_t {
    socket_interface_t() {};

    socket_interface_t(int socket) : holder(socket) {};
    int holder;
};



template<typename __CAN_FRAME_T>
class basic_can_interface {
public:
    basic_can_interface(const char *interface_name) : _interface_name(interface_name) {
        const int protocol = CAN_RAW;
        sockaddr_can addr;
        ifreq ifr = {};
        int can_sock = ::socket(PF_CAN, SOCK_RAW, protocol);
        _interface_name.copy(ifr.ifr_name, _interface_name.size());
        if (ioctl(can_sock, SIOCGIFINDEX, &ifr) < 0) {
            std::string error_info = "[basic_can_interface]Error while syncing io with system.";
            throw std::runtime_error(error_info);
        }
        addr.can_family = AF_CAN;
        addr.can_ifindex = ifr.ifr_ifindex;
        if (bind(can_sock, reinterpret_cast<const sockaddr *>(&addr), sizeof(addr)) < 0) {
            std::string error_info = "[basic_can_interface]Error while opening cansock.";

            throw std::runtime_error(error_info);
        }
        socket_holder = socket_interface_t(can_sock);
    }

    //const bool release(); // release binded socket
    //const error_t open();
    virtual const error_t close() = 0;

    virtual __CAN_FRAME_T read() = 0;

    virtual const error_t write(const __CAN_FRAME_T &frame_to_write) = 0;

    socket_interface_t &socket() noexcept { return socket_holder; };

    const std::string &ifterface_name() { return _interface_name; };

    ~basic_can_interface() {}

private:
    std::string _interface_name;
    socket_interface_t socket_holder;
};


class can_interface : public basic_can_interface<can_frame> {
public:
    can_interface(const char *interface_name) : basic_can_interface<can_frame>::basic_can_interface(interface_name),
                                                socket_holder(socket()) {
        socket_holder.holder;
    }

    const error_t close() override { return 0; }

    can_frame read() override {
        can_frame rev_buffer{};
        std::size_t bytes_read = ::read(socket_holder.holder, &rev_buffer, sizeof(can_frame));
        return rev_buffer;
    };

    const error_t write(const can_frame &frame_to_write) override {
        std::size_t byte_written = ::write(socket_holder.holder, &frame_to_write, sizeof(can_frame));
        return errno;
    };
private:
    socket_interface_t &socket_holder;
};


#endif //EMU_CS_BASIC_CAN_INTERFACE_HPP
