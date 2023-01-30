#pragma once

#include "gl_draw.hpp"
#include "gl_utils.hpp"
#include "gl_viewport.hpp"

class gl_device_ssd1306 {
public:
    enum info {
        SCREEN_WIDTH  = 128,
        SCREEN_HEIGHT = 64,
        SCREEN_COL    = SCREEN_WIDTH,
        SCREEN_ROW    = SCREEN_HEIGHT / 8,
    };
    enum gl_color {
        COLOR_BLACK,
        COLOR_WHITE,
        COLOR_XOR,
    };

public:
    gl_device_ssd1306() {}
    ~gl_device_ssd1306() {}

    void update(void)
    {
    }

protected:
    gl_color basic_get_pixel(gl_uint8 x, gl_uint8 y)
    {
        return ((buffer[y / 8][x] >> (y % 8)) & 1) ? COLOR_WHITE : COLOR_BLACK;
    }

    void basic_set_pixel(gl_uint8 x, gl_uint8 y, gl_color cr)
    {
        _set_pixel(y / 8, 1 << (y % 8), x, cr);
    }

    void basic_fill_rect(gl_uint8 x, gl_uint8 y, gl_uint8 w, gl_uint8 h, gl_color cr)
    {
        if (w == 0 || h == 0) return;

        gl_uint8 row, col, xstep;
        gl_uint8 subrow, ystep, yoffset;

        row = y / 8;

        if ((yoffset = y % 8) > 0) {
            ystep = 8 - yoffset;
            if (h < ystep) ystep = h;
            h -= ystep;
            // <
            col    = x;
            xstep  = w;
            subrow = ~(0xff << ystep) << yoffset;
            while (xstep--) {
                _set_pixel(row, subrow, col, cr);
                ++col;
            };
            // >
            ++row;
        }

        if (h == 0) return;

        // ystep = 8;
        while (h >= 8) {
            h -= 8;
            // <
            col   = x;
            xstep = w;
            // subrow = 0xff;
            while (xstep--) {
                _set_pixel(row, 0xff, col, cr);
                ++col;
            };
            // >
            ++row;
        }

        if (h == 0) return;

        // <
        col    = x;
        xstep  = w;
        subrow = (1 << h) - 1;
        while (xstep--) {
            _set_pixel(row, subrow, col, cr);
            ++col;
        };
        // >
    }

private:
    void _set_pixel(gl_uint8 row, gl_uint8 subrow /* mask */, gl_uint8 col, gl_color cr)
    {
        switch (cr) {
            case COLOR_BLACK: buffer[row][col] &= ~subrow;
            case COLOR_WHITE: buffer[row][col] |= subrow;
            case COLOR_XOR: buffer[row][col] ^= subrow;
        }
    }

    gl_uint8 buffer[SCREEN_ROW][SCREEN_COL] = {0};
};

typedef gl_draw<gl_device_ssd1306, gl_device_ssd1306::gl_color> gl_viewport_easyx;
