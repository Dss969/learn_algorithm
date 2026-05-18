/*
 * ui.c  —  EasyX 界面绘制实现
 *
 * 编译要求：Windows + EasyX安装包
 * 字符编码：UTF-8（VS用户需在项目属性中添加 /utf-8 编译选项）
 */
#include "ui.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>     /* _getch() */
#include <windows.h>   /* Sleep(), SetWindowText() */

#define PI 3.14159265f

/* 对比配色：蓝 / 橙 / 绿 */
COLORREF g_car_colors[3] = {
    RGB(64,  128, 255),
    RGB(255, 160,  32),
    RGB(64,  200,  64)
};

/* ================================================================ */
/* 内部辅助函数                                                       */
/* ================================================================ */

/* 数学角度（度）转屏幕坐标，屏幕 Y 轴朝下所以 sin 取反 */
static void angle_to_xy(int cx, int cy, float r, float deg, int *px, int *py)
{
    float rad = deg * PI / 180.0f;
    *px = cx + (int)(r * (float)cos(rad));
    *py = cy - (int)(r * (float)sin(rad));
}

/*
 * 在 EasyX 窗口中读入一行 ASCII 字符串（带光标回显）
 * 入参: buf 缓冲区, maxlen 最大字符数, (x,y) 输入框屏幕坐标
 * 按 Enter 确认，Esc 将 buf 置空并返回
 */
static void read_string(char *buf, int maxlen, int x, int y)
{
    int pos = 0;
    int ch;
    char tmp[512];

    buf[0] = '\0';
    while (1) {
        /* 清除当行输入区并重绘 */
        setfillcolor(RGB(30, 30, 50));
        solidrectangle(x, y, x + 460, y + 30);
        settextcolor(RGB(220, 220, 220));
        snprintf(tmp, sizeof(tmp), "%s|", buf);   /* | 作为光标 */
        settextstyle(20, 0, "宋体");
        outtextxy(x + 4, y + 4, tmp);
        FlushBatchDraw();

        ch = _getch();
        if (ch == 13)          break;                  /* Enter 确认 */
        if (ch == 27)        { buf[0] = '\0'; return; } /* Esc 取消 */
        if (ch == 8 && pos > 0) { buf[--pos] = '\0'; }  /* Backspace */
        else if (ch >= 32 && pos < maxlen - 1) {
            buf[pos++] = (char)ch;
            buf[pos]   = '\0';
        }
    }
}

/* 读取浮点数：读字符串后转换，输入为空时返回默认值 */
static float read_float(float def_val, int x, int y)
{
    char buf[64];
    buf[0] = '\0';
    read_string(buf, sizeof(buf), x, y);
    if (buf[0] == '\0') return def_val;
    return (float)atof(buf);
}

/* ================================================================ */
/* 生命周期                                                           */
/* ================================================================ */

void ui_init(void)
{
    initgraph(WIN_WIDTH, WIN_HEIGHT);
    SetWindowText(GetHWnd(), "车辆性能仿真系统");
    setbkmode(TRANSPARENT);   /* 文字背景透明 */
    BeginBatchDraw();          /* 开启批量绘图，避免闪烁 */
}

void ui_close(void)
{
    EndBatchDraw();
    closegraph();
}

/* ================================================================ */
/* 帧控制                                                             */
/* ================================================================ */

void ui_begin_frame(void)
{
    setbkcolor(RGB(18, 18, 28));
    cleardevice();
}

void ui_end_frame(void)
{
    FlushBatchDraw();   /* 一次性刷新到屏幕，消除闪烁 */
    Sleep(20);          /* 控制约 50 fps */
}

/* ================================================================ */
/* 主菜单                                                             */
/* ================================================================ */

int ui_main_menu(void)
{
    int i, key;
    int y0 = 235;
    COLORREF clrs[5];
    const char *lines[5];

    clrs[0] = RGB(100, 200, 255);
    clrs[1] = RGB(100, 255, 150);
    clrs[2] = RGB(255, 180, 100);
    clrs[3] = RGB(200, 150, 255);
    clrs[4] = RGB(180, 180, 180);

    lines[0] = "  [1]   加速测试     0 -> 100 km/h 动态仿真 + v-t 曲线";
    lines[1] = "  [2]   制动测试     100 km/h 紧急制动 + s-t 曲线";
    lines[2] = "  [3]   车型对比     多车叠加曲线 + 参数表格";
    lines[3] = "  [4]   车型管理     添加 / 删除车型";
    lines[4] = "  [0]   退    出";

    ui_begin_frame();

    /* 标题 */
    settextstyle(52, 0, "宋体");
    settextcolor(RGB(255, 220, 50));
    outtextxy(WIN_WIDTH / 2 - 230, 58, "车辆性能仿真系统");

    /* 副标题 */
    settextstyle(22, 0, "宋体");
    settextcolor(RGB(150, 150, 175));
    outtextxy(WIN_WIDTH / 2 - 175, 130, "汽车理论公式  +  EasyX 可视化");

    /* 分割线 */
    setlinecolor(RGB(55, 55, 75));
    setlinestyle(PS_SOLID, 1);
    line(140, 178, WIN_WIDTH - 140, 178);

    /* 菜单项 */
    settextstyle(30, 0, "宋体");
    for (i = 0; i < 5; i++) {
        settextcolor(clrs[i]);
        outtextxy(WIN_WIDTH / 2 - 310, y0 + i * 75, lines[i]);
    }

    /* 底部提示 */
    settextstyle(16, 0, "宋体");
    settextcolor(RGB(70, 70, 95));
    outtextxy(WIN_WIDTH / 2 - 110, WIN_HEIGHT - 38, "按键盘数字键选择功能");

    ui_end_frame();

    /* 阻塞等待按键 */
    while (1) {
        key = _getch();
        switch (key) {
            case '1': return 1;
            case '2': return 2;
            case '3': return 3;
            case '4': return 4;
            case '0': case 27: return 0;
            default:  break;
        }
    }
}

/* ================================================================ */
/* 仪表盘（0-260 km/h，270°圆弧）                                     */
/* ================================================================ */

void ui_draw_speedometer(float speed_kmh, float rpm, int cx, int cy, int radius)
{
    int i, px, py, tx, ty;
    float angle, needle_angle;
    char buf[32];
    float max_speed = 260.0f;

    /* --- 外圈 --- */
    setlinecolor(RGB(70, 70, 90));
    setlinestyle(PS_SOLID, 3);
    circle(cx, cy, radius);
    circle(cx, cy, radius - 8);

    /*
     * --- 刻度线 ---
     * 0 km/h 对应 225°（左下），260 km/h 对应 -45°（右下）
     * 顺时针走过 270°圆弧
     */
    for (i = 0; i <= 26; i++) {
        float v   = (float)i * 10.0f;
        float len = (i % 5 == 0) ? 0.14f : 0.07f;

        angle = 225.0f - (v / max_speed) * 270.0f;
        angle_to_xy(cx, cy, (float)radius * (1.0f - len - 0.02f), angle, &px, &py);
        angle_to_xy(cx, cy, (float)radius - 9.0f,                  angle, &tx, &ty);

        if (i % 5 == 0) {
            setlinecolor(RGB(220, 220, 220));
            setlinestyle(PS_SOLID, 2);
        } else {
            setlinecolor(RGB(100, 100, 120));
            setlinestyle(PS_SOLID, 1);
        }
        line(px, py, tx, ty);

        /* 主刻度旁标注数字 */
        if (i % 5 == 0) {
            int lx, ly;
            angle_to_xy(cx, cy, (float)radius * 0.70f, angle, &lx, &ly);
            settextstyle(14, 0, "宋体");
            settextcolor(RGB(190, 190, 190));
            snprintf(buf, sizeof(buf), "%d", (int)v);
            outtextxy(lx - 10, ly - 8, buf);
        }
    }

    /* --- 指针 --- */
    if (speed_kmh < 0.0f)      speed_kmh = 0.0f;
    if (speed_kmh > max_speed) speed_kmh = max_speed;

    needle_angle = 225.0f - (speed_kmh / max_speed) * 270.0f;

    angle_to_xy(cx, cy, (float)radius * 0.18f, needle_angle + 180.0f, &px, &py);
    angle_to_xy(cx, cy, (float)radius * 0.82f, needle_angle,          &tx, &ty);
    setlinecolor(RGB(255, 70, 40));
    setlinestyle(PS_SOLID, 3);
    line(px, py, tx, ty);

    /* 中心圆钉 */
    setfillcolor(RGB(255, 70, 40));
    fillcircle(cx, cy, 7);

    /* --- 速度数字 --- */
    settextstyle(38, 0, "宋体");
    settextcolor(RGB(255, 220, 50));
    snprintf(buf, sizeof(buf), "%.0f", speed_kmh);
    outtextxy(cx - 28, cy + radius / 3, buf);

    settextstyle(14, 0, "宋体");
    settextcolor(RGB(160, 160, 160));
    outtextxy(cx - 22, cy + radius / 3 + 42, "km/h");

    /* --- 转速 --- */
    settextstyle(15, 0, "宋体");
    settextcolor(RGB(120, 190, 255));
    snprintf(buf, sizeof(buf), "%.0f rpm", rpm);
    outtextxy(cx - 30, cy + radius / 3 + 62, buf);
}

/* ================================================================ */
/* 简笔侧视车（矩形 + 圆轮）                                            */
/* ================================================================ */

void ui_draw_car_side(int x, int y, COLORREF color)
{
    /* 车顶 */
    setfillcolor(color);
    solidrectangle(x + 28, y - 30, x + 132, y);

    /* 车身 */
    solidrectangle(x, y, x + 160, y + 42);

    /* 车窗 */
    setfillcolor(RGB(120, 195, 230));
    solidrectangle(x + 36, y - 24, x + 82,  y - 4);
    solidrectangle(x + 88, y - 24, x + 128, y - 4);

    /* 前轮 */
    setfillcolor(RGB(40, 40, 45));
    fillcircle(x + 32,  y + 42, 18);
    setfillcolor(RGB(150, 150, 155));
    fillcircle(x + 32,  y + 42, 8);

    /* 后轮 */
    setfillcolor(RGB(40, 40, 45));
    fillcircle(x + 128, y + 42, 18);
    setfillcolor(RGB(150, 150, 155));
    fillcircle(x + 128, y + 42, 8);
}

/* ================================================================ */
/* 跑道背景                                                             */
/* ================================================================ */

void ui_draw_track(int x, int y, int w, int h)
{
    int i;
    int road_top = y + h / 2 - 35;
    int road_bot = y + h / 2 + 55;
    int mid_y    = (road_top + road_bot) / 2;

    /* 天空 */
    setfillcolor(RGB(18, 30, 65));
    solidrectangle(x, y, x + w, road_top);

    /* 路面 */
    setfillcolor(RGB(55, 55, 65));
    solidrectangle(x, road_top, x + w, road_bot);

    /* 边芒草地 */
    setfillcolor(RGB(25, 70, 25));
    solidrectangle(x, road_bot, x + w, y + h);

    /* 中心虚线（每 40px 画 20px线段） */
    setlinecolor(RGB(240, 230, 90));
    setlinestyle(PS_SOLID, 2);
    for (i = x; i < x + w; i += 40)
        line(i, mid_y, i + 22, mid_y);
}

/* ================================================================ */
/* 通用曲线绘制（内部）                                                 */
/* ================================================================ */

static void draw_axes(const char *xlabel, const char *ylabel,
                      float max_x, float max_y,
                      int ox, int oy, int w, int h)
{
    int i;
    char buf[32];

    setlinecolor(RGB(170, 170, 170));
    setlinestyle(PS_SOLID, 1);
    line(ox, oy, ox + w + 10, oy);   /* X 轴 */
    line(ox, oy, ox, oy - h - 10);   /* Y 轴 */

    settextstyle(14, 0, "宋体");
    settextcolor(RGB(160, 160, 160));

    for (i = 1; i <= 5; i++) {
        int tx = ox + i * w / 5;
        line(tx, oy, tx, oy + 5);
        snprintf(buf, sizeof(buf), "%.1f", max_x * (float)i / 5.0f);
        outtextxy(tx - 12, oy + 8, buf);

        /* 背景网格线 */
        setlinecolor(RGB(38, 38, 52));
        line(tx, oy - h, tx, oy);
        setlinecolor(RGB(170, 170, 170));
    }

    for (i = 1; i <= 4; i++) {
        int ty = oy - i * h / 4;
        line(ox - 5, ty, ox, ty);
        snprintf(buf, sizeof(buf), "%.0f", max_y * (float)i / 4.0f);
        outtextxy(ox - 48, ty - 8, buf);

        setlinecolor(RGB(38, 38, 52));
        line(ox, ty, ox + w, ty);
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
        float t2 = (float)i       * dt;

        px1 = ox + (int)(t1 / max_t * (float)w);
        py1 = oy - (int)(arr[i-1] / max_val * (float)h);
        px2 = ox + (int)(t2 / max_t * (float)w);
        py2 = oy - (int)(arr[i]   / max_val * (float)h);

        line(px1, py1, px2, py2);
    }
}

/* ================================================================ */
/* v-t 曲线                                                           */
/* ================================================================ */

void ui_draw_vt_curve(float v_arr[], int count, float dt, COLORREF color,
                      int ox, int oy, int w, int h)
{
    float max_t  = (float)(count - 1) * dt;
    float max_v  = 40.0f;    /* Y 轴上限 40 m/s ≈ 144 km/h */
    int   ref_y;

    draw_axes("t/s", "v/(m/s)", max_t, max_v, ox, oy, w, h);
    draw_polyline(v_arr, count, dt, max_v, color, ox, oy, w, h);

    /* 100 km/h 参考红虹线 */
    ref_y = oy - (int)(27.78f / max_v * (float)h);
    setlinecolor(RGB(255, 90, 90));
    setlinestyle(PS_DASH, 1);
    line(ox, ref_y, ox + w, ref_y);
    settextstyle(13, 0, "宋体");
    settextcolor(RGB(255, 90, 90));
    outtextxy(ox + 4, ref_y - 17, "100 km/h");
}

/* ================================================================ */
/* s-t 曲线                                                           */
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
/* 对比图例                                                           */
/* ================================================================ */

void ui_draw_comparison_legend(Car cars[], int count, COLORREF colors[])
{
    int i;
    int lx = WIN_WIDTH - 240;
    int ly = 70;

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

/* ================================================================ */
/* 对比表格                                                           */
/* ================================================================ */

void ui_draw_comparison_table(Car cars[], float t100[], float brake_dist[],
                              float vmax[], int count)
{
    int i;
    int tx = 50;
    int ty = WIN_HEIGHT - 190;
    char buf[64];
    int col2 = tx + 280, col3 = tx + 520, col4 = tx + 760;

    /* 表头 */
    setfillcolor(RGB(35, 35, 58));
    solidrectangle(tx, ty, tx + 980, ty + 28);
    settextstyle(18, 0, "宋体");
    settextcolor(RGB(255, 220, 50));
    outtextxy(tx + 8,    ty + 5, "车型");
    outtextxy(col2 + 8,  ty + 5, "0-100 km/h (s)");
    outtextxy(col3 + 8,  ty + 5, "制动距离 (m)");
    outtextxy(col4 + 8,  ty + 5, "理论最高车速 (km/h)");

    setlinecolor(RGB(70, 70, 95));
    line(tx, ty + 28, tx + 980, ty + 28);

    for (i = 0; i < count; i++) {
        int ry = ty + 30 + i * 38;

        if (i % 2 == 0) {
            setfillcolor(RGB(22, 22, 38));
            solidrectangle(tx, ry, tx + 980, ry + 36);
        }

        settextstyle(18, 0, "宋体");
        settextcolor(g_car_colors[i % 3]);
        outtextxy(tx + 8, ry + 8, cars[i].name);

        settextcolor(RGB(210, 210, 210));

        if (t100[i] < 0.0f)
            snprintf(buf, sizeof(buf), "> 30 s");
        else
            snprintf(buf, sizeof(buf), "%.2f", t100[i]);
        outtextxy(col2 + 8, ry + 8, buf);

        snprintf(buf, sizeof(buf), "%.1f", brake_dist[i]);
        outtextxy(col3 + 8, ry + 8, buf);

        snprintf(buf, sizeof(buf), "%.0f", vmax[i] * 3.6f);
        outtextxy(col4 + 8, ry + 8, buf);
    }
}

/* ================================================================ */
/* 车型选择面板                                                        */
/* ================================================================ */

int ui_select_car(Car cars[], int count)
{
    int sel = 0;
    int ch, i;
    char info[128];

    if (count <= 0) return -1;

    while (1) {
        ui_begin_frame();

        settextstyle(28, 0, "宋体");
        settextcolor(RGB(255, 220, 50));
        outtextxy(WIN_WIDTH / 2 - 80, 38, "选择车型");

        settextstyle(15, 0, "宋体");
        settextcolor(RGB(110, 110, 135));
        outtextxy(WIN_WIDTH / 2 - 170, 82,
                  "↑ ↓ 移动高亮   Enter 确认   Esc 取消");

        settextstyle(21, 0, "宋体");
        for (i = 0; i < count; i++) {
            int ry = 124 + i * 48;

            if (i == sel) {
                setfillcolor(RGB(45, 75, 118));
                solidrectangle(WIN_WIDTH / 2 - 330, ry - 4,
                               WIN_WIDTH / 2 + 330, ry + 38);
                settextcolor(RGB(255, 255, 100));
            } else {
                settextcolor(RGB(195, 195, 195));
            }

            snprintf(info, sizeof(info),
                     "%-20s  %.0f kW   %.0f N.m   %.0f kg",
                     cars[i].name,
                     cars[i].power_kw,
                     cars[i].torque_nm,
                     cars[i].mass_kg);
            outtextxy(WIN_WIDTH / 2 - 310, ry + 4, info);
        }

        ui_end_frame();

        /* 按键处理：扩展键先读 0xE0，再读具体键码 */
        ch = _getch();
        if (ch == 0 || ch == 0xE0) {
            ch = _getch();
            if (ch == 72 && sel > 0)           sel--;   /* ↑ */
            if (ch == 80 && sel < count - 1)   sel++;   /* ↓ */
        } else if (ch == 13) {
            return sel;
        } else if (ch == 27) {
            return -1;
        }
    }
}

/* ================================================================ */
/* 参数录入面板                                                        */
/* ================================================================ */

int ui_input_car(Car *out)
{
    /*
     * 逐字段在 EasyX 窗口内输入参数。
     * 车型名仅支持 ASCII（英文/拼音），Enter 确认每字段。
     */
    int lx  = WIN_WIDTH / 2 - 320;
    int vx  = WIN_WIDTH / 2 + 20;
    int y0  = 90;
    int dy  = 52;
    char buf[64];

    memset(out, 0, sizeof(Car));

    /* 绘制所有标签（一次性） */
    ui_begin_frame();
    settextstyle(26, 0, "宋体");
    settextcolor(RGB(255, 220, 50));
    outtextxy(lx, 38, "录入新车型参数");
    settextstyle(15, 0, "宋体");
    settextcolor(RGB(110, 110, 135));
    outtextxy(lx, 68, "依次填写各字段，Enter 确认，Esc 取消当前字段输入");

    {
        /* 定义每一行的标签和单位提示 */
        const char *labels[9] = {
            "车型名称：",
            "额定功率 (kW)：",
            "最大扭矩 (N.m)：",
            "整备质量 (kg)：",
            "风阻系数 Cd：",
            "迎风面积 (m2)：",
            "综合传动比：",
            "轮胎半径 (m)：",
            "制动减速度 (m/s2)："
        };
        const char *hints[9] = {
            "为 0 取消 / 小于32个字符",
            "e.g. 180",
            "e.g. 320",
            "e.g. 1400",
            "e.g. 0.28",
            "e.g. 2.20",
            "e.g. 10.5",
            "e.g. 0.32",
            "e.g. 9.0"
        };
        int row;

        settextstyle(20, 0, "宋体");
        for (row = 0; row < 9; row++) {
            settextcolor(RGB(150, 195, 255));
            outtextxy(lx, y0 + row * dy, labels[row]);
            settextcolor(RGB(100, 100, 120));
            outtextxy(vx + 240, y0 + row * dy + 4, hints[row]);
        }
    }
    FlushBatchDraw();

    /* 车型名 */
    read_string(out->name, sizeof(out->name), vx, y0 + 0 * dy);
    if (out->name[0] == '\0') return 0;

    /* 8 个浮点字段 */
    out->power_kw        = read_float(100.0f, vx, y0 + 1 * dy);
    out->torque_nm       = read_float(300.0f, vx, y0 + 2 * dy);
    out->mass_kg         = read_float(1400.0f,vx, y0 + 3 * dy);
    out->drag_coeff      = read_float(0.28f,  vx, y0 + 4 * dy);
    out->frontal_area_m2 = read_float(2.2f,   vx, y0 + 5 * dy);
    out->gear_ratio      = read_float(10.5f,  vx, y0 + 6 * dy);
    out->wheel_radius_m  = read_float(0.32f,  vx, y0 + 7 * dy);
    out->max_brake_decel = read_float(9.0f,   vx, y0 + 8 * dy);

    /* 确认 */
    ui_begin_frame();
    settextstyle(22, 0, "宋体");
    settextcolor(RGB(100, 255, 100));
    snprintf(buf, sizeof(buf), "车型 [%s] 录入完成  Enter=保存   Esc=放弃", out->name);
    outtextxy(lx, WIN_HEIGHT / 2, buf);
    FlushBatchDraw();

    {
        int ch = _getch();
        return (ch == 13) ? 1 : 0;
    }
}
