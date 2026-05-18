/*
 * ui.c  —  EasyX 界面绘制实现
 *
 * 编译：必须以 C++ 模式编译（g++ -x c++ 或 VS /TP）
 * 编码：源文件 UTF-8，MinGW 加 -fexec-charset=GBK
 */
#include "ui.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#define PI 3.14159265f

COLORREF g_car_colors[3] = {
    RGB(64,  128, 255),
    RGB(255, 160,  32),
    RGB(64,  200,  64)
};

static void angle_to_xy(int cx, int cy, float r, float deg, int *px, int *py)
{
    float rad = deg * PI / 180.0f;
    *px = cx + (int)(r * (float)cos(rad));
    *py = cy - (int)(r * (float)sin(rad));
}

/*
 * 从 EasyX 消息队列读一行 ASCII 字符串。
 * 用 getmessage() 而不是 _getch()：EasyX 默认隐藏控制台，_getch() 无法收到事件。
 */
static void read_string(char *buf, int maxlen, int x, int y)
{
    int pos = 0, done = 0;
    char tmp[512];
    ExMessage msg;

    buf[0] = '\0';
    flushmessage(EM_KEY);
    while (!done) {
        setfillcolor(RGB(30, 30, 50));
        solidrectangle(x, y, x + 460, y + 30);
        settextcolor(RGB(220, 220, 220));
        snprintf(tmp, sizeof(tmp), "%s|", buf);
        settextstyle(20, 0, "宋体");
        outtextxy(x + 4, y + 4, tmp);
        FlushBatchDraw();

        getmessage(&msg, EM_KEY);
        if (msg.message == WM_KEYDOWN) {
            if (msg.vkcode == VK_RETURN)               done = 1;
            else if (msg.vkcode == VK_ESCAPE)        { buf[0] = '\0'; done = 1; }
            else if (msg.vkcode == VK_BACK && pos > 0) buf[--pos] = '\0';
        } else if (msg.message == WM_CHAR) {
            if (msg.ch >= 32 && msg.ch < 127 && pos < maxlen - 1) {
                buf[pos++] = (char)msg.ch;
                buf[pos]   = '\0';
            }
        }
    }
}

static float read_float(float def_val, int x, int y)
{
    char buf[64];
    buf[0] = '\0';
    read_string(buf, sizeof(buf), x, y);
    if (buf[0] == '\0') return def_val;
    return (float)atof(buf);
}

/* ================================================================ */
void ui_init(void)
{
    initgraph(WIN_WIDTH, WIN_HEIGHT);
    SetWindowText(GetHWnd(), "车辆性能仿真系统");
    setbkmode(TRANSPARENT);
    BeginBatchDraw();
}

void ui_close(void)
{
    EndBatchDraw();
    closegraph();
}

void ui_begin_frame(void)
{
    setbkcolor(RGB(18, 18, 28));
    cleardevice();
}

void ui_end_frame(void)
{
    FlushBatchDraw();
    Sleep(20);
}

/* ================================================================ */
int ui_main_menu(void)
{
    int i;
    int y0 = 195, dy = 63;
    COLORREF clrs[7];
    const char *lines[7];
    ExMessage msg;

    clrs[0] = RGB(100, 200, 255);
    clrs[1] = RGB(100, 255, 150);
    clrs[2] = RGB(255, 180, 100);
    clrs[3] = RGB(200, 150, 255);
    clrs[4] = RGB(255, 220, 100);
    clrs[5] = RGB(160, 220, 200);
    clrs[6] = RGB(160, 160, 160);

    lines[0] = "  [1]   加速测试     0->100 km/h 仪表盘动画 + v-t 曲线";
    lines[1] = "  [2]   制动测试     100 km/h 紧急制动 + s-t 曲线";
    lines[2] = "  [3]   车型对比     多车叠加曲线 + 参数表格";
    lines[3] = "  [4]   车型管理     添加 / 删除车型";
    lines[4] = "  [5]   全库排行榜   所有车型加速性能横向排名";
    lines[5] = "  [6]   外特性曲线   驱动力 & 阻力 vs 车速（恒扭矩/恒功率）";
    lines[6] = "  [0]   退    出";

    ui_begin_frame();

    settextstyle(52, 0, "宋体");
    settextcolor(RGB(255, 220, 50));
    outtextxy(WIN_WIDTH / 2 - 230, 44, "车辆性能仿真系统");

    settextstyle(20, 0, "宋体");
    settextcolor(RGB(150, 150, 175));
    outtextxy(WIN_WIDTH / 2 - 160, 114, "汽车理论公式  +  EasyX 可视化");

    setlinecolor(RGB(55, 55, 75));
    setlinestyle(PS_SOLID, 1);
    line(140, 158, WIN_WIDTH - 140, 158);

    settextstyle(26, 0, "宋体");
    for (i = 0; i < 7; i++) {
        settextcolor(clrs[i]);
        outtextxy(WIN_WIDTH / 2 - 310, y0 + i * dy, lines[i]);
    }

    settextstyle(16, 0, "宋体");
    settextcolor(RGB(70, 70, 95));
    outtextxy(WIN_WIDTH / 2 - 110, WIN_HEIGHT - 38, "按键盘数字键选择功能");

    ui_end_frame();

    flushmessage(EM_KEY);
    while (1) {
        getmessage(&msg, EM_KEY);
        if (msg.message == WM_KEYDOWN && msg.vkcode == VK_ESCAPE) return 0;
        if (msg.message == WM_CHAR) {
            switch (msg.ch) {
                case '1': return 1;
                case '2': return 2;
                case '3': return 3;
                case '4': return 4;
                case '5': return 5;
                case '6': return 6;
                case '0': return 0;
                default:  break;
            }
        }
    }
}

/* ================================================================ */
void ui_draw_speedometer(float speed_kmh, float rpm, int cx, int cy, int radius)
{
    int i, px, py, tx, ty;
    float angle, needle_angle;
    char buf[32];
    float max_speed = 260.0f;

    setlinecolor(RGB(70, 70, 90));
    setlinestyle(PS_SOLID, 3);
    circle(cx, cy, radius);
    circle(cx, cy, radius - 8);

    for (i = 0; i <= 26; i++) {
        float v   = (float)i * 10.0f;
        float len = (i % 5 == 0) ? 0.14f : 0.07f;
        angle = 225.0f - (v / max_speed) * 270.0f;
        angle_to_xy(cx, cy, (float)radius * (1.0f - len - 0.02f), angle, &px, &py);
        angle_to_xy(cx, cy, (float)radius - 9.0f, angle, &tx, &ty);
        if (i % 5 == 0) { setlinecolor(RGB(220,220,220)); setlinestyle(PS_SOLID, 2); }
        else             { setlinecolor(RGB(100,100,120)); setlinestyle(PS_SOLID, 1); }
        line(px, py, tx, ty);
        if (i % 5 == 0) {
            int lx, ly;
            angle_to_xy(cx, cy, (float)radius * 0.70f, angle, &lx, &ly);
            settextstyle(14, 0, "宋体");
            settextcolor(RGB(190, 190, 190));
            snprintf(buf, sizeof(buf), "%d", (int)v);
            outtextxy(lx - 10, ly - 8, buf);
        }
    }

    if (speed_kmh < 0.0f)      speed_kmh = 0.0f;
    if (speed_kmh > max_speed) speed_kmh = max_speed;
    needle_angle = 225.0f - (speed_kmh / max_speed) * 270.0f;
    angle_to_xy(cx, cy, (float)radius * 0.18f, needle_angle + 180.0f, &px, &py);
    angle_to_xy(cx, cy, (float)radius * 0.82f, needle_angle,          &tx, &ty);
    setlinecolor(RGB(255, 70, 40));
    setlinestyle(PS_SOLID, 3);
    line(px, py, tx, ty);
    setfillcolor(RGB(255, 70, 40));
    fillcircle(cx, cy, 7);

    settextstyle(38, 0, "宋体");
    settextcolor(RGB(255, 220, 50));
    snprintf(buf, sizeof(buf), "%.0f", speed_kmh);
    outtextxy(cx - 28, cy + radius / 3, buf);
    settextstyle(14, 0, "宋体");
    settextcolor(RGB(160, 160, 160));
    outtextxy(cx - 22, cy + radius / 3 + 42, "km/h");
    settextstyle(15, 0, "宋体");
    settextcolor(RGB(120, 190, 255));
    snprintf(buf, sizeof(buf), "%.0f rpm", rpm);
    outtextxy(cx - 30, cy + radius / 3 + 62, buf);
}

/* ================================================================ */
void ui_draw_car_side(int x, int y, COLORREF color)
{
    setfillcolor(color);
    solidrectangle(x + 28, y - 30, x + 132, y);
    solidrectangle(x, y, x + 160, y + 42);
    setfillcolor(RGB(120, 195, 230));
    solidrectangle(x + 36, y - 24, x + 82,  y - 4);
    solidrectangle(x + 88, y - 24, x + 128, y - 4);
    setfillcolor(RGB(40, 40, 45));
    fillcircle(x + 32,  y + 42, 18);
    setfillcolor(RGB(150, 150, 155));
    fillcircle(x + 32,  y + 42, 8);
    setfillcolor(RGB(40, 40, 45));
    fillcircle(x + 128, y + 42, 18);
    setfillcolor(RGB(150, 150, 155));
    fillcircle(x + 128, y + 42, 8);
}

/* ================================================================ */
void ui_draw_track(int x, int y, int w, int h, int dash_offset)
{
    int i;
    int road_top = y + h / 2 - 35;
    int road_bot = y + h / 2 + 55;
    int mid_y    = (road_top + road_bot) / 2;
    int start;

    setfillcolor(RGB(18, 30, 65));
    solidrectangle(x, y, x + w, road_top);
    setfillcolor(RGB(55, 55, 65));
    solidrectangle(x, road_top, x + w, road_bot);
    setfillcolor(RGB(25, 70, 25));
    solidrectangle(x, road_bot, x + w, y + h);

    setlinecolor(RGB(240, 230, 90));
    setlinestyle(PS_SOLID, 2);
    start = x - (dash_offset % 40);
    for (i = start; i < x + w; i += 40) {
        int x1 = i, x2 = i + 22;
        if (x2 < x) continue;
        if (x1 < x) x1 = x;
        if (x2 > x + w) x2 = x + w;
        line(x1, mid_y, x2, mid_y);
    }
}

/* ================================================================ */
static void draw_axes(const char *xlabel, const char *ylabel,
                      float max_x, float max_y,
                      int ox, int oy, int w, int h)
{
    int i;
    char buf[32];
    setlinecolor(RGB(170, 170, 170));
    setlinestyle(PS_SOLID, 1);
    line(ox, oy, ox + w + 10, oy);
    line(ox, oy, ox, oy - h - 10);
    settextstyle(14, 0, "宋体");
    settextcolor(RGB(160, 160, 160));
    for (i = 1; i <= 5; i++) {
        int tx = ox + i * w / 5;
        line(tx, oy, tx, oy + 5);
        snprintf(buf, sizeof(buf), "%.1f", max_x * (float)i / 5.0f);
        outtextxy(tx - 12, oy + 8, buf);
        setlinecolor(RGB(38, 38, 52)); line(tx, oy - h, tx, oy);
        setlinecolor(RGB(170, 170, 170));
    }
    for (i = 1; i <= 4; i++) {
        int ty = oy - i * h / 4;
        line(ox - 5, ty, ox, ty);
        snprintf(buf, sizeof(buf), "%.0f", max_y * (float)i / 4.0f);
        outtextxy(ox - 48, ty - 8, buf);
        setlinecolor(RGB(38, 38, 52)); line(ox, ty, ox + w, ty);
        setlinecolor(RGB(170, 170, 170));
    }
    settextcolor(RGB(200, 200, 200));
    outtextxy(ox + w + 12, oy - 8,   xlabel);
    outtextxy(ox - 8,      oy - h - 24, ylabel);
}

static void draw_polyline(float arr[], int count, float dt,
                           float max_val, COLORREF color,
                           int ox, int oy, int w, int h)
{
    int i, px1, py1, px2, py2;
    float max_t;
    if (count <= 1 || max_val <= 0.0f) return;
    max_t = (float)(count - 1) * dt;
    setlinecolor(color);
    setlinestyle(PS_SOLID, 2);
    for (i = 1; i < count; i++) {
        float t1 = (float)(i - 1) * dt;
        float t2 = (float)i * dt;
        px1 = ox + (int)(t1 / max_t * (float)w);
        py1 = oy - (int)(arr[i-1] / max_val * (float)h);
        px2 = ox + (int)(t2 / max_t * (float)w);
        py2 = oy - (int)(arr[i]   / max_val * (float)h);
        line(px1, py1, px2, py2);
    }
}

/* ================================================================ */
void ui_draw_vt_curve(float v_arr[], int count, float dt, COLORREF color,
                      float max_v_ms_hint,
                      int ox, int oy, int w, int h)
{
    float max_t = (float)(count - 1) * dt;
    float max_v_ms;
    int k, ticks, ref_y;

    if (max_v_ms_hint > 0.0f) {
        /* 调用方已提供上限（多曲线共享时使用），直接用 */
        max_v_ms = max_v_ms_hint;
    } else {
        /* 自动缩放：取最大速度，向上对齐到 20 km/h 格 */
        max_v_ms = 0.0f;
        for (k = 0; k < count; k++)
            if (v_arr[k] > max_v_ms) max_v_ms = v_arr[k];
        ticks = (int)(max_v_ms * 3.6f / 20.0f) + 1;
        if (ticks < 2) ticks = 2;
        max_v_ms = (float)ticks * 20.0f / 3.6f;
    }

    draw_axes("t/s", "v/(m/s)", max_t, max_v_ms, ox, oy, w, h);
    draw_polyline(v_arr, count, dt, max_v_ms, color, ox, oy, w, h);

    if (27.78f <= max_v_ms) {
        ref_y = oy - (int)(27.78f / max_v_ms * (float)h);
        setlinecolor(RGB(255, 90, 90));
        setlinestyle(PS_DASH, 1);
        line(ox, ref_y, ox + w, ref_y);
        settextstyle(13, 0, "宋体");
        settextcolor(RGB(255, 90, 90));
        outtextxy(ox + 4, ref_y - 17, "100 km/h");
    }
}

/* ================================================================ */
void ui_draw_st_curve(float s_arr[], int count, float dt, COLORREF color,
                      int ox, int oy, int w, int h)
{
    float max_t = (float)(count - 1) * dt;
    float max_s = (count > 0) ? s_arr[count - 1] : 1.0f;
    if (max_s < 1.0f) max_s = 1.0f;
    draw_axes("t/s", "s/m", max_t, max_s, ox, oy, w, h);
    draw_polyline(s_arr, count, dt, max_s, color, ox, oy, w, h);
}

/* ================================================================ */
void ui_draw_comparison_legend(Car cars[], int count, COLORREF colors[])
{
    int i;
    int lx = WIN_WIDTH - 240, ly = 70;
    settextstyle(17, 0, "宋体");
    settextcolor(RGB(180, 180, 180));
    outtextxy(lx, ly - 22, "图例");
    for (i = 0; i < count; i++) {
        setfillcolor(colors[i]);
        solidrectangle(lx, ly + i * 32, lx + 28, ly + i * 32 + 18);
        settextcolor(colors[i]);
        outtextxy(lx + 36, ly + i * 32, cars[i].name);
    }
}

void ui_draw_comparison_table(Car cars[], float t100[], float brake_dist[],
                              float vmax[], int count)
{
    int i;
    int tx = 50, ty = WIN_HEIGHT - 190;
    char buf[64];
    int col2 = tx + 280, col3 = tx + 520, col4 = tx + 760;

    setfillcolor(RGB(35, 35, 58));
    solidrectangle(tx, ty, tx + 980, ty + 28);
    settextstyle(18, 0, "宋体");
    settextcolor(RGB(255, 220, 50));
    outtextxy(tx + 8,   ty + 5, "车型");
    outtextxy(col2 + 8, ty + 5, "0-100 km/h (s)");
    outtextxy(col3 + 8, ty + 5, "制动距离 (m)");
    outtextxy(col4 + 8, ty + 5, "理论极速 (km/h)");
    setlinecolor(RGB(70, 70, 95));
    line(tx, ty + 28, tx + 980, ty + 28);

    for (i = 0; i < count; i++) {
        int ry = ty + 30 + i * 38;
        if (i % 2 == 0) { setfillcolor(RGB(22,22,38)); solidrectangle(tx, ry, tx+980, ry+36); }
        settextstyle(18, 0, "宋体");
        settextcolor(g_car_colors[i % 3]);
        outtextxy(tx + 8, ry + 8, cars[i].name);
        settextcolor(RGB(210, 210, 210));
        if (t100[i] < 0.0f) snprintf(buf, sizeof(buf), "> 30 s");
        else                 snprintf(buf, sizeof(buf), "%.2f", t100[i]);
        outtextxy(col2 + 8, ry + 8, buf);
        snprintf(buf, sizeof(buf), "%.1f", brake_dist[i]);
        outtextxy(col3 + 8, ry + 8, buf);
        snprintf(buf, sizeof(buf), "%.0f", vmax[i] * 3.6f);
        outtextxy(col4 + 8, ry + 8, buf);
    }
}

/* ================================================================ */
int ui_select_car(Car cars[], int count)
{
    int sel = 0, i;
    char info[128];
    ExMessage msg;
    if (count <= 0) return -1;
    flushmessage(EM_KEY);
    while (1) {
        ui_begin_frame();
        settextstyle(28, 0, "宋体");
        settextcolor(RGB(255, 220, 50));
        outtextxy(WIN_WIDTH / 2 - 80, 38, "选择车型");
        settextstyle(15, 0, "宋体");
        settextcolor(RGB(110, 110, 135));
        outtextxy(WIN_WIDTH / 2 - 170, 82, "↑ ↓ 移动高亮   Enter 确认   Esc 取消");
        settextstyle(21, 0, "宋体");
        for (i = 0; i < count; i++) {
            int ry = 124 + i * 48;
            if (i == sel) {
                setfillcolor(RGB(45, 75, 118));
                solidrectangle(WIN_WIDTH/2-330, ry-4, WIN_WIDTH/2+330, ry+38);
                settextcolor(RGB(255, 255, 100));
            } else {
                settextcolor(RGB(195, 195, 195));
            }
            snprintf(info, sizeof(info), "%-20s  %.0f kW   %.0f N.m   %.0f kg",
                     cars[i].name, cars[i].power_kw, cars[i].torque_nm, cars[i].mass_kg);
            outtextxy(WIN_WIDTH / 2 - 310, ry + 4, info);
        }
        ui_end_frame();
        while (peekmessage(&msg, EM_KEY, 1)) {
            if (msg.message == WM_KEYDOWN) {
                if (msg.vkcode == VK_UP   && sel > 0)         sel--;
                if (msg.vkcode == VK_DOWN && sel < count - 1) sel++;
                if (msg.vkcode == VK_RETURN) { flushmessage(EM_KEY); return sel; }
                if (msg.vkcode == VK_ESCAPE) { flushmessage(EM_KEY); return -1; }
            }
        }
    }
}

/* ================================================================ */
int ui_input_car(Car *out)
{
    int lx = WIN_WIDTH/2 - 320, vx = WIN_WIDTH/2 + 20;
    int y0 = 90, dy = 52;
    char buf[64];
    ExMessage msg;
    const char *labels[9] = {
        "车型名称：","额定功率 (kW)：","最大扭矩 (N.m)：","整备质量 (kg)：",
        "风阻系数 Cd：","迎风面积 (m2)：","综合传动比：","轮胎半径 (m)：","制动减速度 (m/s2)："
    };
    const char *hints[9] = {
        "< 32字符","e.g. 180","e.g. 320","e.g. 1400",
        "e.g. 0.28","e.g. 2.20","e.g. 10.5","e.g. 0.32","e.g. 9.0"
    };
    int row;

    memset(out, 0, sizeof(Car));
    ui_begin_frame();
    settextstyle(26, 0, "宋体"); settextcolor(RGB(255,220,50));
    outtextxy(lx, 38, "录入新车型参数");
    settextstyle(15, 0, "宋体"); settextcolor(RGB(110,110,135));
    outtextxy(lx, 68, "依次填写各字段，Enter 确认，Esc 取消当前字段");
    settextstyle(20, 0, "宋体");
    for (row = 0; row < 9; row++) {
        settextcolor(RGB(150,195,255)); outtextxy(lx, y0+row*dy, labels[row]);
        settextcolor(RGB(100,100,120)); outtextxy(vx+240, y0+row*dy+4, hints[row]);
    }
    FlushBatchDraw();

    read_string(out->name, sizeof(out->name), vx, y0);
    if (out->name[0] == '\0') return 0;
    out->power_kw        = read_float(100.0f, vx, y0 + 1*dy);
    out->torque_nm       = read_float(300.0f, vx, y0 + 2*dy);
    out->mass_kg         = read_float(1400.0f,vx, y0 + 3*dy);
    out->drag_coeff      = read_float(0.28f,  vx, y0 + 4*dy);
    out->frontal_area_m2 = read_float(2.2f,   vx, y0 + 5*dy);
    out->gear_ratio      = read_float(10.5f,  vx, y0 + 6*dy);
    out->wheel_radius_m  = read_float(0.32f,  vx, y0 + 7*dy);
    out->max_brake_decel = read_float(9.0f,   vx, y0 + 8*dy);

    ui_begin_frame();
    settextstyle(22, 0, "宋体"); settextcolor(RGB(100,255,100));
    snprintf(buf, sizeof(buf), "车型 [%s] 录入完成  Enter=保存   Esc=放弃", out->name);
    outtextxy(lx, WIN_HEIGHT/2, buf);
    FlushBatchDraw();
    flushmessage(EM_KEY);
    getmessage(&msg, EM_KEY);
    return (msg.message == WM_KEYDOWN && msg.vkcode == VK_RETURN) ? 1 : 0;
}
