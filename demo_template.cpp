#include "gl_vp_easyx.hpp"

#include <math.h>
#include <conio.h>  // getch

// gl_viewport_easyx vp;

int main()
{
    gl_viewport_easyx vp{};

    vp.fill_screen(WHITE);

    vp.update();

    getch();

    return 0;
}
