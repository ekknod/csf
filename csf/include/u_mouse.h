#ifndef U_MOUSE_H
#define U_MOUSE_H

#include "../../rx/include/rx_input.h"

extern "C" int usleep(unsigned int microseconds);

class u_mouse {
    rx_handle _dev;
public:
    u_mouse(RX_INPUT_MODE input_mode = RX_INPUT_MODE_ALL)
    {
        _dev = rx_open_input(RX_INPUT_TYPE_MOUSE, input_mode);
        if (_dev == 0)
            throw "error: u_mouse::initialize";
    }
    ~u_mouse() { rx_close_handle(_dev); }
    inline void click(RX_BUTTON_CODE button)
    {
        rx_send_input_button(_dev, button, 1);
        usleep(10);
        rx_send_input_button(_dev, button, 0);
    }
    inline rx_bool button_down(RX_BUTTON_CODE button)
    {
        return rx_button_down(_dev, button);
    }
    inline vec2_i axis(void)
    {
        return rx_mouse_axis(_dev);
    }
    inline void move_x(int x)
    {
        rx_send_input_axis(_dev, RX_MOUSE_X, x);
    }
    inline void move_y(int y)
    {
        rx_send_input_axis(_dev, RX_MOUSE_Y, y);
    }
    inline void move(int x, int y)
    {
        move_x(x);
        move_y(y);
    }
} ;

#endif // U_MOUSE_H

