

#include "gl_vp_easyx.hpp"

#include <math.h>
#include <conio.h>  // getch

int main()  // 卷积
{
    gl_viewport_easyx _vp{}, vp{};

    gl_rect box = vp.screen.part().scale(0.4f, 0.5f).set_center(vp.screen.center());

    vp.fill_screen(WHITE);
    vp.draw_rect(box, RED);

    _vp.fill_screen(BLACK);
    _vp.draw_rect(box, WHITE);

    // 边缘提取

#if 1

    gl_int8 sobel_hor_core[3][3] = {
        {-1, 0, +1},
        {-2, 0, +2},
        {-1, 0, +1},
    };

    _vp.conv((gl_int8*)sobel_hor_core, 3, 3, 1, 1);

#else

    gl_int8 sobel_ver_core[3][3] = {
        {+1, +2, +1},
        {0, 0, 0},
        {-1, -2, -1},
    };

    _vp.conv((gl_int8*)sobel_ver_core, 3, 3, 1, 1);

#endif

    vp.draw(_vp, _vp.screen.part(Selection_LeftTop), vp.screen.origin());
    vp.draw(_vp, _vp.screen.part(Selection_RightBottom), vp.screen.center());

    vp.update();

    getch();

    return 0;
}
