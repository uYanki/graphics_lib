
#pragma once

#include "gl_utils.hpp"

class gl_point {
public:
    gl_point(gl_int x = 0, gl_int y = 0) : x(x), y(y) {}

    gl_int x;
    gl_int y;
};

class gl_pointf {
public:
    gl_pointf(gl_real x = 0.f, gl_real y = 0.f) : x(x), y(y) {}

    gl_real x;
    gl_real y;
};

class gl_rect {
public:
    gl_rect(void) : m_left(0), m_top(0), m_right(0), m_bottom(0) {}
    gl_rect(gl_int left, gl_int top, gl_uint width, gl_uint height)
        : m_left(left), m_top(top), m_right(left + width), m_bottom(top + height) {}
    ~gl_rect() {}

    //**> attributions <**//

    gl_int left(void) const { return m_left; }
    gl_int top(void) const { return m_top; }
    gl_int right(void) const { return m_right; }
    gl_int bottom(void) const { return m_bottom; }
    gl_int width(void) const { return m_right - m_left; }
    gl_int height(void) const { return m_bottom - m_top; }
    gl_int hcenter(void) const { return (m_left + m_right) / 2; }
    gl_int vcenter(void) const { return (m_top + m_bottom) / 2; }

    gl_point origin(void) const { return gl_point(left(), top()); }
    gl_point size(void) const { return gl_point(width(), height()); }
    gl_point center(void) const { return gl_point(hcenter(), vcenter()); }
    gl_int   area(void) const { return width() * height(); }

    gl_rect part(gl_selection opt = Selection_ALL) const
    {
        switch (opt) {
            case Selection_Left: return gl_rect(left(), top(), width() / 2, height());
            case Selection_Right: return gl_rect(hcenter(), top(), width() / 2, height());
            case Selection_Top: return gl_rect(left(), top(), width(), height() / 2);
            case Selection_Bottom: return gl_rect(left(), vcenter(), width(), height() / 2);
            case Selection_LeftTop: return gl_rect(left(), top(), width() / 2, height() / 2);
            case Selection_RightTop: return gl_rect(hcenter(), top(), width() / 2, height() / 2);
            case Selection_LeftBottom: return gl_rect(left(), vcenter(), width() / 2, height() / 2);
            case Selection_RightBottom: return gl_rect(hcenter(), vcenter(), width() / 2, height() / 2);
            case Selection_ALL: return gl_rect(*this);
        }
        return gl_rect();  // null rect
    }

    bool is_empty(void) const
    {
        return m_left >= m_right || m_top >= m_bottom;
    }

    gl_rect& set_origin(const gl_int x = 0, const gl_int y = 0)
    {
        m_right  = x + width();
        m_bottom = y + height();
        m_left   = x;
        m_top    = y;
        return *this;
    }
    gl_rect& set_size(const gl_int w = 0, const gl_int h = 0)
    {
        m_right  = m_left + w;
        m_bottom = m_top + h;
        return *this;
    }
    gl_rect& set_center(const gl_int x = 0, const gl_int y = 0)
    {
        gl_int dx = width() / 2, dy = height() / 2;
        m_left   = x - dx;
        m_top    = y - dy;
        m_right  = x + dx;
        m_bottom = y + dy;
        return *this;
    }

    gl_rect& set_center(const gl_point& pt)
    {
        return set_center(pt.x, pt.y);
    }

    //**> functions <**//

    gl_rect& normalize(void)
    {
        if (m_left > m_right) gl_swap(&m_left, &m_right);
        if (m_top > m_bottom) gl_swap(&m_top, &m_bottom);
        return *this;
    }
    gl_rect& transpose(void)
    {
        gl_swap(&m_left, &m_top);
        gl_swap(&m_right, &m_bottom);
        return *this;
    }

    gl_rect& offset(const gl_int dx, const gl_int dy)
    {
        m_left += dx;
        m_right += dx;
        m_bottom += dy;
        m_top += dy;
        return *this;
    }
    gl_rect& inflate(const gl_int dx, const gl_int dy)
    {
        m_left -= dx;
        m_right += dx;
        m_top -= dy;
        m_bottom += dy;
        return *this;
    }
    gl_rect& deflate(const gl_int dx, const gl_int dy)
    {
        return inflate(-dx, -dy);
    }
    gl_rect& scale(const gl_real sx, const gl_real sy)
    {
        if (sx != 1.0f) m_right = m_left + width() * sx;
        if (sy != 1.0f) m_bottom = m_top + height() * sy;
        return *this;
        // dpi -> scale x,y
    }
    // _rounded_scale();
    // _ceiled_scale();
    // _floored_scale();

    // (left,top) is inside, but (right,bottom) is not
    bool contains(const gl_int x, const gl_int y) const
    {
        return m_left <= x && m_top <= y && m_right > x && m_bottom > y;
    }
    bool contains(const gl_point& pt) const
    {
        return contains(pt.x, pt.y);
    }
    bool contains(const gl_rect& rc) const
    {
        return (rc.m_left >= m_left && rc.m_right >= m_right && rc.m_bottom <= m_bottom && rc.m_right <= m_right);
    }

    gl_rect& union_with(const gl_rect& other);  // 并集

    // will ignore empty rect
    bool intersect_with(const gl_rect& other, bool set = false);  // 交集

    gl_rect& align(const gl_rect& ref, const gl_align hor, const gl_align ver);

    gl_rect& print(void)
    {
        printf("(x1=%d,y1=%d,x2=%d,y2=%d) w=%d,h=%d\n", m_left, m_top, m_right, m_bottom, width(), height());
        return *this;
    }

protected:
    gl_int m_left;
    gl_int m_top;
    gl_int m_right;
    gl_int m_bottom;
};

gl_rect& gl_rect::union_with(const gl_rect& other)
{
    if (other.m_left < m_left) m_left = other.m_left;
    if (other.m_top < m_top) m_top = other.m_top;
    if (other.m_right > m_right) m_right = other.m_right;
    if (other.m_bottom > m_bottom) m_bottom = other.m_bottom;

    return *this;
}

bool gl_rect::intersect_with(const gl_rect& other, bool set)
{
    gl_int left  = gl_max(m_left, other.m_left),
           right = gl_min(m_right, other.m_right);

    if (left < right) {
        gl_int top    = gl_max(m_top, other.m_top),
               bottom = gl_min(m_bottom, other.m_bottom);

        if (top < bottom) {
            if (set) {
                m_left   = left;
                m_top    = top;
                m_right  = right;
                m_bottom = bottom;
            }
            return true;
        }
    }

    if (set) {  // empty rect
        m_left = m_top = m_right = m_bottom = 0;
    }
    return false;
}

gl_rect& gl_rect::align(const gl_rect& ref, const gl_align hor, const gl_align ver)
{
    gl_int w = width(), h = height();

    switch (hor) {
        case ALIGN_INNER_LEFT: {
            m_left  = ref.m_left;
            m_right = m_left + w;
            break;
        }
        case ALIGN_INNER_RIGHT: {
            m_right = ref.m_right;
            m_left  = m_right - w;
            break;
        }
        case ALIGN_OUTER_LEFT: {
            m_right = ref.m_left;
            m_left  = m_right - w;
            break;
        }
        case ALIGN_OUTER_RIGHT: {
            m_left  = ref.m_right;
            m_right = m_left + w;
            break;
        }
        case ALIGN_HCENTER: {
            m_left  = ref.hcenter() - w / 2;
            m_right = m_left + w;
            break;
        }
    }

    switch (ver) {
        case ALIGN_INNER_TOP: {
            m_top    = ref.m_top;
            m_bottom = m_top + h;
            break;
        }
        case ALIGN_INNER_BOTTOM: {
            m_bottom = ref.m_bottom;
            m_top    = m_bottom - h;
            break;
        }
        case ALIGN_OUTER_TOP: {
            m_bottom = ref.m_top;
            m_top    = m_bottom - h;
            break;
        }
        case ALIGN_OUTER_BOTTOM: {
            m_top    = ref.m_bottom;
            m_bottom = m_top + h;
            break;
        }
        case ALIGN_VCENTER: {
            m_top    = ref.vcenter() - h / 2;
            m_bottom = m_top + h;
            break;
        }
    }

    return *this;
}

gl_rect intersect_rect(const gl_rect& src1, const gl_rect& src2)
{
    gl_rect t(src1);
    return t.intersect_with(src2, true) ? t : gl_rect();
}

gl_rect union_rect(const gl_rect& src1, const gl_rect& src2)
{
    return gl_rect(src1).union_with(src2);
}

void rotate(gl_point pts[], gl_uint8 cnt, gl_real angle)
{
    int j;

    gl_real s = gl_sin(angle),
            c = gl_cos(angle);
    while (cnt--) {
        gl_int x = pts[cnt].x, y = pts[cnt].y;
        pts[cnt].x = x * c + y * s;
        pts[cnt].y = -x * s + y * c;
    }
}
