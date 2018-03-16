# C++ template style CAN-Bus interfaces

Provide C++ interface to both can_frame and canfd_frame.
## USAGE

Include the basic_can_interface.hpp into your project and do something like:

```C++
using namespace CAN;
try {
  can_interface can("can1");
  auto buffer = can.read();
  can_frame buffer_send = {.can_id = 0x700,
    .can_dlc = 3,
    .data = {
	1,2,3
    }
  };
  can.send(buffer_send);
} catch (can_exception_t & e) {
  std::cout << e.what() << std::endl;
  // Some error handler, like open the socket again or other things you like
} catch (...) {
  std::cout << e.what() << std::endl;
  throw; // A-O, I just re-throw it :)
}
```

## CONTRIBUTING
Feel free to open a bug issue or some feature request. MR is welcom.

## LICENCE
This source is under Apache License 2.0, please check "LICENSE" file if you like.
