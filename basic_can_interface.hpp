//
// Created by sleeplessy on 2/24/18.
//

#ifndef EMU_CS_BASIC_CAN_INTERFACE_HPP
#define EMU_CS_BASIC_CAN_INTERFACE_HPP

#include <atomic>
#include <exception>
#include <linux/can.h>
#include <net/if.h>
#include <stdexcept>
#include <string>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

namespace CAN {
struct socket_interface_t {
  socket_interface_t(){};

  socket_interface_t(int socket) : holder(socket){};
  std::atomic<int> holder;
};

// EXCEPTIONS
class can_exception_t : public std::runtime_error {
public:
  can_exception_t(std::string error_info)
      : runtime_error(error_info.insert(0, "CAN INTERFACE EXCEPTION: ")) {}

  can_exception_t() : runtime_error("CAN INTERFACE EXCEPTION: UNKNOW"){};
};

class can_init_error : public can_exception_t {
public:
  can_init_error() : can_exception_t("INIT FAILED.") {}

  can_init_error(std::string &error_info)
      : can_exception_t(std::string("INIT FAILED.").append(error_info)) {}
};

class can_length_unmatch : public can_exception_t {
public:
  can_length_unmatch() : can_exception_t("LENGTH OF DATA UNSUPPOSED.") {}
};

template <typename __CAN_FRAME_T> class basic_can_interface {
public:
  basic_can_interface(const char *interface_name)
      : _interface_name(interface_name) {
    const int protocol = CAN_RAW;
    sockaddr_can addr;
    ifreq ifr = {};
    int can_sock = ::socket(PF_CAN, SOCK_RAW, protocol);
    _interface_name.copy(ifr.ifr_name, _interface_name.size());
    if (ioctl(can_sock, SIOCGIFINDEX, &ifr) < 0) {
      throw can_init_error();
    }
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    if (bind(can_sock, reinterpret_cast<const sockaddr *>(&addr),
             sizeof(addr)) < 0) {

      throw can_init_error();
    }
    socket_holder.holder.store(can_sock);
  }

  virtual void close() = 0;

  virtual __CAN_FRAME_T read() = 0;

  virtual void write(const __CAN_FRAME_T &frame_to_write) = 0;

  socket_interface_t &socket() noexcept { return socket_holder; };

  const std::string &ifterface_name() { return _interface_name; };

  ~basic_can_interface() {}

private:
  std::string _interface_name;
  socket_interface_t socket_holder;
};

class can_interface : public basic_can_interface<can_frame> {
public:
  can_interface(const char *interface_name)
      : basic_can_interface<can_frame>::basic_can_interface(interface_name),
        socket_holder(socket()) {
    socket_holder.holder;
  }

  void close() override { ::close(socket_holder.holder); }

  can_frame read() override {
    can_frame rev_buffer{};
    if (::read(socket_holder.holder, &rev_buffer, sizeof(can_frame)) <
        sizeof(can_frame))
      throw can_length_unmatch();
    return rev_buffer;
  };

  void write(const can_frame &frame_to_write) override {
    if (::write(socket_holder.holder, &frame_to_write, sizeof(can_frame)) !=
        sizeof(can_frame))
      throw can_length_unmatch();
  };

private:
  socket_interface_t &socket_holder;
};

} // namespace CAN
#endif // EMU_CS_BASIC_CAN_INTERFACE_HPP
