#pragma once

#include "gl_draw.hpp"

#include <easyx.h>
#include <graphics.h>

class gl_device_easyx {
public:
    enum info {
        SCREEN_WIDTH  = 200,
        SCREEN_HEIGHT = 200,
        SCREEN_COL    = SCREEN_WIDTH,
        SCREEN_ROW    = SCREEN_HEIGHT,
    };
    typedef uint32_t gl_color;

public:
    gl_device_easyx()
    {
        initgraph(SCREEN_WIDTH, SCREEN_HEIGHT, SHOWCONSOLE);
    }
    ~gl_device_easyx()
    {
        closegraph();
    }

    void update()
    {
        BeginBatchDraw();
        for (gl_uint16 x = 0; x < SCREEN_COL; ++x)
            for (gl_uint16 y = 0; y < SCREEN_ROW; ++y)
                putpixel(x, y, buffer[x][y]);
        EndBatchDraw();
    }

protected:
    // gl_color* const operator[](const gl_uint16 i)
    // {
    //     return buffer[i];
    // }

    gl_color basic_get_pixel(gl_uint16 x, gl_uint16 y)
    {
        return buffer[x][y];
    }

    void basic_set_pixel(gl_uint16 x, gl_uint16 y, gl_color cr)
    {
        buffer[x][y] = cr;
    }

    void basic_fill_rect(gl_uint16 x, gl_uint16 y, gl_uint16 w, gl_uint16 h, gl_color cr)
    {
        gl_uint32 x1, y1, x2 = x + w, y2 = y + h;
        for (y1 = y; y1 < y2; ++y1)
            for (x1 = x; x1 < x2; ++x1)
                buffer[x1][y1] = cr;
    }

public:
    gl_color buffer[SCREEN_COL][SCREEN_ROW] = {0};
};

typedef gl_draw<gl_device_easyx, gl_device_easyx::gl_color> gl_viewport_easyx;
