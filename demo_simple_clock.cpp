#include "gl_vp_easyx.hpp"

#include <math.h>
#include <conio.h>  // getch

int main()
{
    gl_viewport_easyx vp{};

    vp.fill_screen(WHITE);

    gl_uint w = vp.screen.width(), h = vp.screen.height();

    gl_uint
        center_x = w / 2,  // 秒针起始坐标
        center_y = h / 2;

    // gl_uint
    //     center_x = vp.screen.hcenter(),
    //     center_y = vp.screen.vcenter();

    gl_int
        secondEnd_x,
        secondEnd_y,  // 秒针终点坐标
        minuteEnd_x,
        minuteEnd_y,  // 分针终点坐标
        hourEnd_x,
        hourEnd_y;  // 时针终点坐标

    gl_uint
        secondLength = w / 4,    // 秒针长度
        minuteLength = w / 5.5,  // 分针长度
        hourLength   = w / 7;    // 时针长度

    gl_real
        secondAngle = 0,  // 秒针对应转到角度
        minuteAngle = 0,  // 分针对应转到角度
        hourAngle   = 0;  // 时针对应转到角度

    SYSTEMTIME ti;  // 系统时间

    while (1) {
        vp.fill_circle(center_x, center_y, secondLength + 30, WHITE);
        vp.draw_circle(center_x, center_y, secondLength + 30, 0x555555);
        vp.draw_circle(center_x, center_y, secondLength + 15, 0x555555);
        for (int i = 0; i < 12; i++) {
            int x = center_x + cos(i * 30.0 / 360 * 2 * PI) * (secondLength + 15.0);
            int y = center_y - sin(i * 30.0 / 360 * 2 * PI) * (secondLength + 15.0);
            vp.fill_circle(x, y, 2, RED);
        }
        GetLocalTime(&ti);
        secondAngle = (ti.wSecond / 60.0) * (2 * PI);
        minuteAngle = (ti.wMinute / 60.0) * (2 * PI);
        hourAngle   = ((ti.wHour % 12) / 12.0) * (2 * PI) + (ti.wMinute / 60.0) * (2 * PI / 12.0);
        secondEnd_x = center_x + secondLength * sin(secondAngle);
        secondEnd_y = center_y - secondLength * cos(secondAngle);
        minuteEnd_x = center_x + minuteLength * sin(minuteAngle);
        minuteEnd_y = center_y - minuteLength * cos(minuteAngle);
        hourEnd_x   = center_x + hourLength * sin(hourAngle);
        hourEnd_y   = center_y - hourLength * cos(hourAngle);
        vp.draw_line(center_x, center_y, secondEnd_x, secondEnd_y, BLACK);  // 画秒针
        vp.draw_line(center_x, center_y, minuteEnd_x, minuteEnd_y, GREEN);  // 画分针
        vp.draw_line(center_x, center_y, hourEnd_x, hourEnd_y, RED);        // 画时针

        vp.update();
    }

    getch();

    return 0;
}
