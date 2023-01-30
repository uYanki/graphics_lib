#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <iostream>
#include <easyx.h>
#include <graphics.h>
#include <math.h>
using namespace std;
// 给出最大值
float maxi(float arr[], int n)
{
    float m = 0;
    for (int i = 0; i < n; ++i)
        if (m < arr[i])
            m = arr[i];
    return m;
}
// 给出最小值
float mini(float arr[], int n)
{
    float m = 1;
    for (int i = 0; i < n; ++i)
        if (m > arr[i])
            m = arr[i];
    return m;
}

void liang_barsky_clipper(float xmin, float ymin, float xmax, float ymax, float x1, float y1, float x2, float y2)
{
    // 定义pi qi为可见的
    float p1 = -(x2 - x1);
    float p2 = -p1;
    float p3 = -(y2 - y1);
    float p4 = -p3;
    float q1 = x1 - xmin;
    float q2 = xmax - x1;
    float q3 = y1 - ymin;
    float q4 = ymax - y1;
    float posarr[5], negarr[5];
    int   posind = 1, negind = 1;
    posarr[0] = 1;
    negarr[0] = 0;
    rectangle(xmin, 467 - ymin, xmax, 467 - ymax);     // 裁剪窗口
    if ((p1 == 0 && q1 < 0) || (p3 == 0 && q3 < 0)) {  // 情况1：p =0，q<0,  直线与裁剪框平行
        outtextxy(80, 80, "线条与窗口平行");
        return;
    }
    if (p1 != 0) {
        float r1 = q1 / p1;
        float r2 = q2 / p2;
        if (p1 < 0) {               // 情况2：p <0，直线从边界的外部延伸到内部
            negarr[negind++] = r1;  // -p1加入负数组
            posarr[posind++] = r2;  // p2加入正数组
        } else {
            negarr[negind++] = r2;
            posarr[posind++] = r1;
        }
    }
    if (p3 != 0) {
        float r3 = q3 / p3;
        float r4 = q4 / p4;
        if (p3 < 0) {  // 情况3：p <0，直线从边界的内部延伸到外部
            negarr[negind++] = r3;
            posarr[posind++] = r4;
        } else {
            negarr[negind++] = r4;
            posarr[posind++] = r3;
        }
    }
    float xn1, yn1, xn2, yn2;
    float rn1, rn2;
    rn1 = maxi(negarr, negind);  // 负数组最大值
    rn2 = mini(posarr, posind);  // 正数组最小值
    xn1 = x1 + p2 * rn1;
    yn1 = y1 + p4 * rn1;  // 计算新点
    xn2 = x1 + p2 * rn2;
    yn2 = y1 + p4 * rn2;
    setcolor(CYAN);                        // 裁剪在边界外的为红色
    line(xn1, 467 - yn1, xn2, 467 - yn2);  // 裁剪在边界内的为懒色
    setlinecolor(RED);
    line(x1, 467 - y1, xn1, 467 - yn1);
    line(x2, 467 - y2, xn2, 467 - yn2);
}

int main()
{
    printf("输入窗口的坐标(wxmin、 wxmax、 wymin、 wymax)\n");  // 自定义边界和线段
    float xmin = 40, xmax = 40, ymin = 100, ymax = 100;
    // scanf("%f %f %f %f", &xmin, &ymin, &xmax, &ymax);

    printf(" 输入行的终点(x1，y1)和(x2，y2) :\n");
    float x1=20, y1=60, x2=80, y2=80;
    int gd = DETECT, gm;
    initgraph(200,200);
    liang_barsky_clipper(xmin, ymin, xmax, ymax, x1, y1, x2, y2);  // 调用 梁-barsky算法
    getch();                                                       // 关闭界面
    closegraph();
}
