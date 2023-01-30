#pragma once

#include "gl_draw.hpp"

template <typename gl_device>
class gl_viewport : public gl_device, public gl_draw<gl_device::gl_color> {
private:
public:
    gl_viewport() : T(), gl_draw<gl_color>(SCREEN_WIDTH, SCREEN_HEIGHT) {}
    ~gl_viewport() {}

private:
    // interface
    void basic_set_pixel(gl_int x, gl_int y, gl_color cr) { _draw_pixel(x, y, cr); }
    void basic_fill_rect(gl_int x, gl_int y, gl_uint w, gl_uint h, gl_color cr) { _fill_rect(x, y, w, h, cr); }
};
