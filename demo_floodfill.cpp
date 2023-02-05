#include "gl_vp_easyx.hpp"

#include <conio.h>  // getch

int main()
{
    gl_viewport_easyx vp{};

    vp.fill_screen(WHITE);

#if 1
    gl_rect box(20, 20, 10, 10);

    for (gl_uint8 i = 0; i < 8; ++i)
        vp.fill_rect(box.offset(0, box.height()), BLACK);
    for (gl_uint8 i = 0; i < 8; ++i)
        vp.fill_rect(box.offset(box.width(), 0), BLACK);
    for (gl_uint8 i = 0; i < 8; ++i)
        vp.fill_rect(box.offset(0, -box.height()), BLACK);
    for (gl_uint8 i = 0; i < 8; ++i)
        vp.fill_rect(box.offset(-box.width(), 0), BLACK);

    gl_rect box_1(box), box_2(box);
    box_1.offset(0, box.height() * 4);
    box_2.offset(box.width() * 4, 0);

    box.offset(box.width() * 8, box.height() * 8);
    gl_rect box_3(box), box_4(box);
    box_3.offset(0, -box.height() * 4);
    box_4.offset(-box.width() * 4, 0);

    for (gl_uint8 i = 0; i < 2; ++i) {
        vp.fill_rect(box_1.offset(box.width(), 0), BLACK);
        vp.fill_rect(box_2.offset(0, box.height()), BLACK);
        vp.fill_rect(box_3.offset(-box.width(), 0), BLACK);
        vp.fill_rect(box_4.offset(0, -box.height()), BLACK);
    }

    vp.fill_rect(box_2.offset(-box.width(), box.height()), BLACK);
    vp.fill_rect(box_4.offset(box.width(), -box.height()), BLACK);

    box.offset(-box.width() * 4, -box.height() * 4);
    // vp.fill_rect(box, BLACK);

    vp.flood_fill(box.hcenter(), box.vcenter(), WHITE, RED, false);
    // vp.flood_fill(box.hcenter(), box.vcenter(), WHITE, RED, true);

#else

    gl_rect box(vp.screen);

    box.deflate(20, 20);

    vp.draw_circle(box.hcenter(), box.vcenter(), box.width() / 3, BLACK);

    gl_uint offset = box.width() / 5;
    vp.draw_circle(box.hcenter() - offset, box.vcenter(), offset / 3, BLACK);
    vp.draw_circle(box.hcenter() + offset, box.vcenter(), offset / 3, BLACK);

    vp.draw_vline(0, 0, vp.screen.vcenter(), GREEN);
    vp.scanline_fill(box.hcenter(), box.vcenter(), WHITE, RED);
    // vp.scanline_fill(box.hcenter(), box.vcenter() - offset, WHITE, RED);

#endif

    vp.update();

    getch();

    return 0;
}
