#pragma once

#include "gl_rect.hpp"

template <class gl_device, typename gl_color, typename = typename std::enable_if<std::is_class<gl_device>::value>::type>
class gl_draw : public gl_device {
public:
    struct gl_image {
        gl_uint16 width;
        gl_uint16 height;
        gl_color* buffer;
    };

    gl_draw(void) : gl_device() {}
    ~gl_draw() {}

public:
    void set_clip(gl_rect rc)
    {
        m_cliprect.intersect_with(rc, true);
    }

    void reset_clip(gl_rect rc)
    {
        m_cliprect = screen;
    }

    void draw(gl_draw& other, const gl_rect& src, const gl_point& dst = gl_point(0, 0))
    {
        gl_rect rc(dst.x, dst.y, src.width(), src.height());
        if (rc.intersect_with(m_cliprect, true)) {
            gl_uint w    = rc.width(),
                    dstx = rc.left(),
                    srcx = src.left() + (dstx - dst.x);
            while (w--) {
                gl_uint h    = rc.height(),
                        dsty = rc.top(),
                        srcy = src.top() + (dsty - dst.y);
                while (h--) {
                    gl_device::basic_set_pixel(
                        dstx, dsty,
                        other.gl_device::basic_get_pixel(srcx, srcy));
                    ++srcy;
                    ++dsty;
                }
                ++srcx;
                ++dstx;
            }
        }
    }

    void draw_pixel(gl_int x, gl_int y, gl_color cr)
    {
#if 1
        if (m_cliprect.contains(x, y))
            gl_device::basic_set_pixel(x, y, cr);
#else
        fill_rect(x, y, 1, 1, cr);
#endif
    }

    void draw_hline(gl_int x, gl_int y, gl_uint w, gl_color cr)
    {
        fill_rect(x, y, w, 1, cr);
    }
    void draw_vline(gl_int x, gl_int y, gl_uint h, gl_color cr)
    {
        fill_rect(x, y, 1, h, cr);
    }

    void draw_line(gl_int x1, gl_int y1, gl_int x2, gl_int y2, gl_color cr)
    {
#if 1

        // bresenam: https://zhuanlan.zhihu.com/p/30553006

        gl_int dx = gl_abs(x2 - x1), sx = x1 < x2 ? 1 : -1;
        gl_int dy = gl_abs(y2 - y1), sy = y1 < y2 ? 1 : -1;
        gl_int err = (dx > dy ? dx : -dy) >> 1;

        while (draw_pixel(x1, y1, cr), x1 != x2 || y1 != y2) {
            gl_int e2 = err;
            if (e2 > -dx) {
                err -= dy;
                x1 += sx;
            }
            if (e2 < dy) {
                err += dx;
                y1 += sy;
            }
        }
#else

        // dda: https://blog.csdn.net/tomatoaaaa/article/details/107699876

        gl_int  dx = x2 - x1, dy = y2 - y1;
        gl_int  len  = gl_max(gl_abs(dx), gl_abs(dy));
        gl_real incx = (gl_real)dx / len, incy = (gl_real)dy / len;
        gl_real x = x1, y = y1;

        for (gl_uint i = 1; i <= len; i++) {
            draw_pixel(gl_int(x + 0.5f), gl_int(y + 0.5f), cr);
            x += incx;
            y += incy;
        }

#endif
    }

#if 0
    void test_draw_line(void)
    {
        // const gl_uint32 W = width;
        // const gl_uint32 H = height;

        const gl_uint min = gl_min(screen.width(), screen.height());

        float cx = screen.width() * 0.5f, cy = screen.height() * 0.5f;

        for (int j = 0; j < 5; j++) {
            float r1 = min * (j + 0.5f) * 0.085f;
            float r2 = min * (j + 1.5f) * 0.085f;
            float t = j * PI / 64.0f, r = (j + 1) * 1.0f;
            for (int i = 1; i <= 64; i++, t += 2.0f * PI / 64.0f) {
                float ct = gl_cos(t), st = gl_sin(t);
                draw_line(cx + r1 * ct, cy - r1 * st, cx + r2 * ct, cy - r2 * st, r);
            }
        }
    }
#endif

    void draw_rect(gl_rect rc, gl_color cr)
    {
        draw_rect(rc.left(), rc.top(), rc.width(), rc.height(), cr);
    }
    void fill_rect(gl_rect rc, gl_color cr)
    {
        if (rc.intersect_with(m_cliprect, true))
            gl_device::basic_fill_rect(rc.left(), rc.top(), rc.width(), rc.height(), cr);
    }

    void draw_rect(gl_int x, gl_int y, gl_uint w, gl_uint h, gl_color cr)
    {
        draw_hline(x, y, w, cr);
        draw_hline(x, y + h - 1, w, cr);
        draw_vline(x, y, h, cr);
        draw_vline(x + w - 1, y, h, cr);
    }
    void fill_rect(gl_int x, gl_int y, gl_uint w, gl_uint h, gl_color cr)
    {
        fill_rect(gl_rect(x, y, w, h), cr);
    }

    void draw_arc(gl_int xc, gl_int yc, gl_uint r, gl_real start_angle, gl_real end_angle, gl_color cr)
    {
        if (!m_cliprect.intersect_with(gl_rect(xc - r, yc - r, r << 1, r << 1))) return;

        if (end_angle < start_angle)
            end_angle += 2 * PI;

        gl_real dt = 0.4f / r;
        gl_uint n  = (gl_uint)((end_angle - start_angle) / dt);
        gl_real a  = start_angle;

        for (gl_uint i = 0; i < n; ++i) {
            a += dt;
            draw_pixel(
                roundf(xc + r * gl_cos(a)),
                roundf(yc + r * gl_sin(a)),
                cr);
        }
    }

    void draw_circle(gl_int xc, gl_int yc, gl_uint r, gl_color cr, gl_selection opt = Selection_ALL)
    {
        paint_circle_section(xc, yc, r, cr, opt, &gl_draw::draw_circle_section);
    }
    void fill_circle(gl_int xc, gl_int yc, gl_uint r, gl_color cr, gl_selection opt = Selection_ALL)
    {
        paint_circle_section(xc, yc, r, cr, opt, &gl_draw::fill_circle_section);
    }

    void draw_ellipse(gl_int xc, gl_int yc, gl_uint rx, gl_uint ry, gl_color cr, gl_selection opt = Selection_ALL)
    {
        paint_ellipse_section(xc, yc, rx, ry, cr, opt, &gl_draw::draw_ellipse_section);
    }
    void fill_ellipse(gl_int xc, gl_int yc, gl_uint rx, gl_uint ry, gl_color cr, gl_selection opt = Selection_ALL)
    {
        paint_ellipse_section(xc, yc, rx, ry, cr, opt, &gl_draw::fill_ellipse_section);
    }

    void draw_pie(gl_int xc, gl_int yc, gl_uint r, gl_real start_angle, gl_real end_angle, gl_color cr)
    {
        draw_arc(xc, yc, r, start_angle, end_angle, cr);
        draw_line(xc, yc, gl_round(xc + r * gl_cos(start_angle)), gl_round(yc + r * gl_sin(start_angle)), cr);
        draw_line(xc, yc, gl_round(xc + r * gl_cos(end_angle)), gl_round(yc + r * gl_sin(end_angle)), cr);
    }
    // void fill_pie(gl_int xc, gl_int yc, gl_uint r, gl_real start_angle, gl_real end_angle,  gl_color cr);

    void draw_roundrect(gl_rect rc, gl_uint r, gl_color cr)
    {
        draw_roundrect(rc.left(), rc.top(), rc.width(), rc.height(), r, cr);
    }
    void fill_roundrect(gl_rect rc, gl_uint r, gl_color cr)
    {
        fill_roundrect(rc.left(), rc.top(), rc.width(), rc.height(), r, cr);
    }

    void draw_roundrect(gl_int x, gl_int y, gl_uint w, gl_uint h, gl_uint r, gl_color cr)
    {
        if (!m_cliprect.intersect_with(gl_rect(x, y, w, h))) return;

        gl_uint xl = x + r,
                yu = y + r,
                xr = x + w - r - 1,
                yl = y + h - r - 1;

        draw_circle(xl, yu, r, cr, Selection_LeftTop);
        draw_circle(xr, yu, r, cr, Selection_RightTop);
        draw_circle(xl, yl, r, cr, Selection_LeftBottom);
        draw_circle(xr, yl, r, cr, Selection_RightBottom);

        gl_uint ww = w - (r << 1),
                hh = h - (r << 1);

        ++xl;
        ++yu;

        if (ww >= 3) {
            ww -= 2;
            --h;
            draw_hline(xl, y, ww, cr);
            draw_hline(xl, y + h, ww, cr);
        }

        if (hh >= 3) {
            hh -= 2;
            --w;
            draw_vline(x, yu, hh, cr);
            draw_vline(x + w, yu, hh, cr);
        }
    }
    void fill_roundrect(gl_int x, gl_int y, gl_uint w, gl_uint h, gl_uint r, gl_color cr)
    {
        if (!m_cliprect.intersect_with(gl_rect(x, y, w, h))) return;

        gl_uint xl = x + r,
                yu = y + r,
                xr = x + w - r - 1,
                yl = y + h - r - 1;

        fill_circle(xl, yu, r, cr, Selection_LeftTop);
        fill_circle(xr, yu, r, cr, Selection_RightTop);
        fill_circle(xl, yl, r, cr, Selection_LeftBottom);
        fill_circle(xr, yl, r, cr, Selection_RightBottom);

        gl_uint ww = w - (r << 1),
                hh = h - (r << 1);

        ++xl;
        ++yu;

        if (ww >= 3) {
            ww -= 2;
            fill_rect(xl, y, ww, r + 1, cr);
            fill_rect(xl, yl, ww, r + 1, cr);
        }

        if (hh >= 3) {
            hh -= 2;
            fill_rect(x, yu, w, hh, cr);
        }
    }

    void draw_polyline(gl_point pts[], gl_uint8 cnt, gl_color cr)
    {
        for (gl_uint8 i = 1; i < cnt; ++i) draw_line(pts[i - 1].x, pts[i - 1].y, pts[i].x, pts[i].y, cr);
    }
    void draw_polyline(gl_int x[], gl_int y[], gl_uint8 cnt, gl_color cr)
    {
        for (gl_uint8 i = 1; i < cnt; ++i) draw_line(x[i - 1], y[i - 1], x[i], y[i], cr);
    }
    // void draw_polybezier();

    void draw_triangle(gl_int x0, gl_int y0, gl_int x1, gl_int y1, gl_int x2, gl_int y2, gl_color cr)
    {
        draw_line(x0, y0, x1, y1, cr);
        draw_line(x1, y1, x2, y2, cr);
        draw_line(x2, y2, x0, y0, cr);
    }
    void fill_triangle(gl_int x0, gl_int y0, gl_int x1, gl_int y1, gl_int x2, gl_int y2, gl_color cr)
    {
        gl_int a, b, y, last;

        if (y0 > y1) {
            gl_swap(&y0, &y1);
            gl_swap(&x0, &x1);
        }
        if (y1 > y2) {
            gl_swap(&y2, &y1);
            gl_swap(&x2, &x1);
        }
        if (y0 > y1) {
            gl_swap(&y0, &y1);
            gl_swap(&x0, &x1);
        }

        if (y0 == y2) {
            a = b = x0;
            if (x1 < a) {
                a = x1;
            } else if (x1 > b) {
                b = x1;
            }
            if (x2 < a) {
                a = x2;
            } else if (x2 > b) {
                b = x2;
            }
            draw_hline(a, y0, b - a + 1, cr);
            return;
        }

        gl_int dx01 = x1 - x0,
               dy01 = y1 - y0,
               dx02 = x2 - x0,
               dy02 = y2 - y0,
               dx12 = x2 - x1,
               dy12 = y2 - y1;

        gl_int sa = 0, sb = 0;

        last = y1;
        if (y1 == y2)
            --last;

        for (y = y0; y <= last; ++y) {
            a = x0 + sa / dy01;
            b = x0 + sb / dy02;
            sa += dx01;
            sb += dx02;
            if (a > b) gl_swap(&a, &b);
            draw_hline(a, y, b - a + 1, cr);
        }

        sa = dx12 * (y - y1);
        sb = dx02 * (y - y0);

        for (; y <= y2; ++y) {
            a = x1 + sa / dy12;
            b = x0 + sb / dy02;
            sa += dx12;
            sb += dx02;
            if (a > b) gl_swap(&a, &b);
            draw_hline(a, y, b - a + 1, cr);
        }
    }

    void draw_polygon(gl_point pts[], gl_uint8 cnt, gl_color cr)
    {
        draw_polyline(pts, cnt, cr);
        draw_line(pts[0].x, pts[0].y, pts[cnt - 1].x, pts[cnt - 1].y, cr);
    }
    void draw_polygon(gl_int x[], gl_int y[], gl_uint8 cnt, gl_color cr)
    {
        draw_polyline(x, y, cnt, cr);
        draw_line(x[0], y[0], x[cnt - 1], y[cnt - 1], cr);
    }

    void fill_polygon(gl_pointf pts[], gl_uint8 cnt, gl_color cr)
    {
        // 扫描线算法

        struct {
            gl_real xmin;
            gl_int  ymin;
            gl_int  ymax;
            gl_real dx;  // dx=1/k
        } side_arr[8];   // 边表（成员数需大于等于顶点数）

        gl_uint8 side_cnt = 0;

        int ymin = m_cliprect.top(), ymax = m_cliprect.bottom();
        for (gl_uint8 i = 0; i < cnt; ++i) {
            if (ymin > pts[i].y) ymin = pts[i].y;
            if (ymax < pts[i].y) ymax = pts[i].y;
        }

        for (gl_int y = ymin; y < ymax; ++y) {
            for (gl_uint8 n = 0; n < cnt; ++n) {
                if (pts[n].y == y) {
                    gl_int8 a = n - 1, b = n + 1;
                    if (a < 0) a += cnt;  // 顶点索引成环
                    if (b < 0) b += cnt;

                    if (pts[a].y > pts[n].y) {
                        side_arr[side_cnt].xmin = pts[n].x;
                        side_arr[side_cnt].ymin = pts[n].y;
                        side_arr[side_cnt].ymax = pts[a].y;
                        side_arr[side_cnt].dx   = (pts[a].x - pts[n].x) / (pts[a].y - pts[n].y);
                        if (pts[b].y <= pts[n].y) ++side_arr[side_cnt].ymin;
                        ++side_cnt;
                    }

                    if (pts[b].y > pts[n].y) {
                        side_arr[side_cnt].xmin = pts[n].x;
                        side_arr[side_cnt].ymin = pts[n].y;
                        side_arr[side_cnt].ymax = pts[b].y;
                        side_arr[side_cnt].dx   = (pts[b].x - pts[n].x) / (pts[b].y - pts[n].y);
                        if (pts[a].y <= pts[n].y) ++side_arr[side_cnt].ymin;
                        ++side_cnt;
                    }
                }
            }
        }

        // printf("t=%d", t);

        gl_uint8 segment_cnt = 0;
        gl_float segment_arr[12];  // 元素个数需大于等于横线与多边形的最大交点数

        for (gl_int y = ymin; y < ymax; ++y) {
            for (gl_uint8 n = 0; n < side_cnt; n++) {
                if (side_arr[n].ymin <= y && side_arr[n].ymax >= y) {
                    segment_arr[segment_cnt++] = side_arr[n].xmin;
                    side_arr[n].xmin += side_arr[n].dx;
                }
            }
            if (segment_cnt > 0) {
                qsort(segment_arr, segment_cnt, sizeof(gl_float), [](const void* f1, const void* f2) -> int {
                    if (*(gl_float*)f1 > *(gl_float*)f2) return 1;
                    if (*(gl_float*)f1 < *(gl_float*)f2) return -1;
                    return 0;
                });
                for (gl_uint i = 0; i < segment_cnt; i += 2) {
                    gl_real a = segment_arr[i];
                    gl_real b = segment_arr[i + 1];
#if 0
                    // 锯齿严重，但绘制速度快
                    draw_hline(a, y, b - a, cr);
#else
                    // 这个锯齿没那么严重
                    for (gl_int x = a + 0.5; x <= b + 0.5; x++)
                        draw_pixel(x, y, cr);
#endif
                }
                segment_cnt = 0;
            }
        }
    }

    void draw_str();
    void draw_num();

    void draw_image();

    void fill_screen(gl_color cr)
    {
        gl_device::basic_fill_rect(m_cliprect.left(), m_cliprect.top(), m_cliprect.width(), m_cliprect.height(), cr);
        // fill_rect(m_cliprect, cr);
        // fill_rect(0, 0, width, height, cr);
    }

    // 泛洪算法（未支持剪辑区） FloodFIll/SeedFill: https://blog.csdn.net/mmm_jsw/article/details/83715031

    void flood_fill(gl_int x, gl_int y, gl_color old_cr, gl_color new_cr, bool opt = false /* false:四领域,true:八领域*/)
    {
        if (new_cr == old_cr) return;  // avoid infinite loop

#if 0
        // 递归(可能会溢出)
        _flood_fill(x, y, old_cr, new_cr, opt);
#else
        // 栈( 栈大小小于 screen.area() 时, 可能渲染不全 )

        if (screen.contains(x, y) && gl_device::basic_get_pixel(x, y) == old_cr) {
            static const gl_int8 dx[8] = {0, 1, 0, -1, 1, 1, -1, -1};
            static const gl_int8 dy[8] = {-1, 0, 1, 0, -1, 1, 1, -1};

            // const gl_uint size = 200;
            const gl_uint size = screen.area();  // >0

            const gl_uint cnt = opt ? 8 : 4;

            gl_uint index = 0;
            gl_int  stack_x[size], stack_y[size];

            // push
            stack_x[index] = x;
            stack_y[index] = y;
            ++index;

            while (index--) {
                // pop
                gl_device::basic_set_pixel(x = stack_x[index], y = stack_y[index], new_cr);
                for (gl_uint8 i = 0; i < cnt; i++) {
                    gl_int nx = x + dx[i], ny = y + dy[i];
                    if (screen.contains(nx, ny) && gl_device::basic_get_pixel(nx, ny) == old_cr) {
                        // push
                        stack_x[index] = nx;
                        stack_y[index] = ny;
                        if (++index == size)
                            break;
                    }
                }

                // printf("%d\n", index);
            }
        }

#endif
    }

    // 扫描线算法（未支持剪辑区）

    void scanline_fill(gl_int x, gl_int y, gl_color old_cr, gl_color new_cr)
    {
        if (new_cr == old_cr) return;
        if (!screen.contains(x, y)) return;
        if (gl_device::basic_get_pixel(x, y) != old_cr) return;

#if 0

        // 递归算法

        _scanline_fill(x, y, old_cr, new_cr);

#else

        // 栈算法

        static const gl_uint w = screen.width(), h = screen.height();

        const gl_uint size = 255;  //>0
        // const gl_uint size = screen.area();

        gl_uint idx = 0;
        gl_int  stack_x[size];
        gl_int  stack_y[size];

        stack_x[idx] = x;
        stack_y[idx] = y;
        ++idx;

        bool span_above, span_below;

        while (idx--) {
            // pop
            x = stack_x[idx];
            y = stack_y[idx];

            while (x > -1 && gl_device::basic_get_pixel(x, y) == old_cr) --x;

            span_above = span_below = false;

            while (++x < w && gl_device::basic_get_pixel(x, y) == old_cr) {
                gl_device::basic_set_pixel(x, y, new_cr);

                if (y > 0) {
                    if (span_above) {
                        if (gl_device::basic_get_pixel(x, y - 1) != old_cr) {
                            span_above = false;
                        }
                    } else {
                        if (gl_device::basic_get_pixel(x, y - 1) == old_cr) {
                            // push
                            stack_x[idx] = x;
                            stack_y[idx] = y - 1;
                            if (++idx == size) break;
                            span_above = true;
                        }
                    }
                }

                if (y < h - 1) {
                    if (span_below) {
                        if (gl_device::basic_get_pixel(x, y + 1) != old_cr) {
                            span_below = false;
                        }
                    } else {
                        if (gl_device::basic_get_pixel(x, y + 1) == old_cr) {
                            // push
                            stack_x[idx] = x;
                            stack_y[idx] = y + 1;
                            if (++idx == size) break;
                            span_below = true;
                        }
                    }
                }
            }
        }

#endif
    }

    // 卷积（外围补零）
    void conv(
        gl_int8* core,  // arr[core_w][core_h]
        gl_uint8 w,
        gl_uint8 h,  // core size
        gl_uint8 xc = 0,
        gl_uint8 yc = 0  // core center
    )
    {
        if (xc >= w || yc >= h) return;

        gl_color buff[screen.width()][screen.height()] = {0};

        gl_uint x, y;

        gl_int8 idx, dx, dy;  // for core
        gl_int  sum;

        const gl_uint8 size = w * h;

        w -= xc;
        h -= yc;

        for (x = 0; x < screen.width(); ++x) {
            for (y = 0; y < screen.height(); ++y) {
                idx = sum = 0;
                for (dx = -xc; dx < w; ++dx)
                    for (dy = -yc; dy < h; ++dy)
                        if (screen.contains(x + dx, y + dy))
                            sum += gl_device::color_to_gray(gl_device::basic_get_pixel(x + dx, y + dy)) * core[idx++];
                buff[x][y] = sum / size;
            }
        }

        for (x = 0; x < screen.width(); ++x)
            for (y = 0; y < screen.height(); ++y)
                gl_device::basic_set_pixel(x, y, gl_device::gray_to_color(buff[x][y]));
    }

private:
    typedef void (gl_draw::*painter_circle_section)(gl_int x, gl_int y, gl_int r, gl_int yc, gl_color cr, gl_selection opt);
    typedef void (gl_draw::*painter_ellipse_section)(gl_int x, gl_int y, gl_int xc, gl_int yc, gl_color cr, gl_selection opt);

    void draw_circle_section(gl_int x, gl_int y, gl_int xc, gl_int yc, gl_color cr, gl_selection opt)
    {
        if (opt & Selection_RightTop) {
            draw_pixel(xc + x, yc - y, cr);
            draw_pixel(xc + y, yc - x, cr);
        }
        if (opt & Selection_LeftTop) {
            draw_pixel(xc - x, yc - y, cr);
            draw_pixel(xc - y, yc - x, cr);
        }
        if (opt & Selection_RightBottom) {
            draw_pixel(xc + x, yc + y, cr);
            draw_pixel(xc + y, yc + x, cr);
        }
        if (opt & Selection_LeftBottom) {
            draw_pixel(xc - x, yc + y, cr);
            draw_pixel(xc - y, yc + x, cr);
        }
    }
    void fill_circle_section(gl_int x, gl_int y, gl_int xc, gl_int yc, gl_color cr, gl_selection opt)
    {
        if (opt & Selection_RightTop) {
            draw_vline(xc + x, yc - y, y + 1, cr);
            draw_vline(xc + y, yc - x, x + 1, cr);
        }
        if (opt & Selection_LeftTop) {
            draw_vline(xc - x, yc - y, y + 1, cr);
            draw_vline(xc - y, yc - x, x + 1, cr);
        }
        if (opt & Selection_RightBottom) {
            draw_vline(xc + x, yc, y + 1, cr);
            draw_vline(xc + y, yc, x + 1, cr);
        }
        if (opt & Selection_LeftBottom) {
            draw_vline(xc - x, yc, y + 1, cr);
            draw_vline(xc - y, yc, x + 1, cr);
        }
    }
    void paint_circle_section(gl_int xc, gl_int yc, gl_uint r, gl_color cr, gl_selection opt, painter_circle_section painter)
    {
        if (!m_cliprect.intersect_with(gl_rect(xc - r, yc - r, r << 1, r << 1))) return;

        gl_int  f  = 1 - r;
        gl_int  dx = 1, dy = -(r << 1);
        gl_uint x = 0, y = r;

        (this->*painter)(x, y, xc, yc, cr, opt);
        while (x < y) {
            if (f >= 0) {
                --y;
                dy += 2;
                f += dy;
            }
            ++x;
            dx += 2;
            f += dx;
            (this->*painter)(x, y, xc, yc, cr, opt);
        }
    }

    void draw_ellipse_section(gl_int x, gl_int y, gl_int xc, gl_int yc, gl_color cr, gl_selection opt)
    {
        if (opt & Selection_RightTop)
            draw_pixel(xc + x, yc - y, cr);
        if (opt & Selection_LeftTop)
            draw_pixel(xc - x, yc - y, cr);
        if (opt & Selection_RightBottom)
            draw_pixel(xc + x, yc + y, cr);
        if (opt & Selection_LeftBottom)
            draw_pixel(xc - x, yc + y, cr);
    }
    void fill_ellipse_section(gl_int x, gl_int y, gl_int xc, gl_int yc, gl_color cr, gl_selection opt)
    {
        if (opt & Selection_RightTop)
            draw_vline(xc + x, yc - y, y + 1, cr);
        if (opt & Selection_LeftTop)
            draw_vline(xc - x, yc - y, y + 1, cr);
        if (opt & Selection_RightBottom)
            draw_vline(xc + x, yc, y + 1, cr);
        if (opt & Selection_LeftBottom)
            draw_vline(xc - x, yc, y + 1, cr);
    }
    void paint_ellipse_section(gl_int xc, gl_int yc, gl_uint rx, gl_uint ry, gl_color cr, gl_selection opt, painter_ellipse_section painter)
    {
        if (!m_cliprect.intersect_with(gl_rect(xc - rx, yc - ry, rx << 1, ry << 1))) return;

        gl_uint x, y;
        gl_long xchg, ychg;
        gl_long err;
        gl_long stopx, stopy;

        gl_long rxrx2 = (rx * rx) << 1,
                ryry2 = (ry * ry) << 1;

        x = rx;
        y = 0;

        xchg = (1 - (rx << 1)) * ry * ry;
        ychg = rx * rx;

        err = 0;

        stopx = ryry2 * rx;
        stopy = 0;

        while (stopx >= stopy) {
            (this->*painter)(x, y, xc, yc, cr, opt);
            ++y;
            stopy += rxrx2;
            err += ychg;
            ychg += rxrx2;
            if ((err << 1) + xchg > 0) {
                --x;
                stopx -= ryry2;
                err += xchg;
                xchg += ryry2;
            }
        }

        x = 0;
        y = ry;

        xchg = ry * ry;
        ychg = (1 - (ry << 1)) * rx * rx;

        err = 0;

        stopx = 0;
        stopy = rxrx2 * ry;

        while (stopx <= stopy) {
            (this->*painter)(x, y, xc, yc, cr, opt);
            ++x;
            stopx += ryry2;
            err += xchg;
            xchg += ryry2;
            if ((err << 1) + ychg > 0) {
                --y;
                stopy -= rxrx2;
                err += ychg;
                ychg += rxrx2;
            }
        }
    }

    void _flood_fill(gl_int x, gl_int y, gl_color old_cr, gl_color new_cr, bool opt)
    {
        if (screen.contains(x, y)) {
            if (gl_device::basic_get_pixel(x, y) == old_cr) {
                gl_device::basic_set_pixel(x, y, new_cr);
                _flood_fill(x + 1, y, old_cr, new_cr, opt);
                _flood_fill(x - 1, y, old_cr, new_cr, opt);
                _flood_fill(x, y + 1, old_cr, new_cr, opt);
                _flood_fill(x, y - 1, old_cr, new_cr, opt);
                if (opt) {
                    _flood_fill(x + 1, y + 1, old_cr, new_cr, opt);
                    _flood_fill(x - 1, y - 1, old_cr, new_cr, opt);
                    _flood_fill(x - 1, y + 1, old_cr, new_cr, opt);
                    _flood_fill(x + 1, y - 1, old_cr, new_cr, opt);
                }
            }
        }
    }

    void _scanline_fill(gl_int x, gl_int y, gl_color old_cr, gl_color new_cr)
    {
        static const gl_uint w = screen.width(), h = screen.height();

        gl_int x1 = x;
        while (x1 < w && gl_device::basic_get_pixel(x1, y) == old_cr) {
            gl_device::basic_set_pixel(x1, y, new_cr);
            ++x1;
        }
        x1 = x - 1;
        while (x1 >= 0 && gl_device::basic_get_pixel(x1, y) == old_cr) {
            gl_device::basic_set_pixel(x1, y, new_cr);
            --x1;
        }
        x1 = x;
        while (x1 < w && gl_device::basic_get_pixel(x1, y) == new_cr) {
            if (y < h - 1 && gl_device::basic_get_pixel(x1, y + 1) == old_cr)
                _scanline_fill(x1, y + 1, old_cr, new_cr);
            ++x1;
        }
        x1 = x - 1;
        while (x1 > 0 && gl_device::basic_get_pixel(x1, y) == new_cr) {
            if (y > 0 && gl_device::basic_get_pixel(x1, y + 1) == old_cr)
                _scanline_fill(x1, y + 1, old_cr, new_cr);
            --x1;
        }
        x1 = x;
        while (x1 < w && gl_device::basic_get_pixel(x1, y) == new_cr) {
            if (y < h - 1 && gl_device::basic_get_pixel(x1, y - 1) == old_cr)
                _scanline_fill(x1, y - 1, old_cr, new_cr);
            ++x1;
        }
        x1 = x - 1;
        while (x1 > 0 && gl_device::basic_get_pixel(x1, y) == new_cr) {
            if (y > 0 && gl_device::basic_get_pixel(x1, y - 1) == old_cr)
                _scanline_fill(x1, y - 1, old_cr, new_cr);
            --x1;
        }
    }

public:
    const gl_rect screen{0, 0, gl_device::SCREEN_WIDTH, gl_device::SCREEN_HEIGHT};

private:
    gl_rect m_cliprect = screen;
};
