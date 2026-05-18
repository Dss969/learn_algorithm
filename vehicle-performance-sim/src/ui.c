/*
 * ui.c  —  EasyX 界面绘制实现
 *
 * 【第三阶段实现，当前为函数存根】
 * 下一提交将逐一实现仪表盘、跑道、曲线等绘制函数。
 */
#include "ui.h"
#include <stdio.h>

/* 对比配色：蓝色 / 橙色 / 绿色 */
COLORREF g_car_colors[3] = {
    RGB(64,  128, 255),   /* 蓝 */
    RGB(255, 160,  32),   /* 橙 */
    RGB(64,  200,  64)    /* 绿 */
};

void ui_init(void)
{
    /* TODO: 第三阶段
     *   initgraph(WIN_WIDTH, WIN_HEIGHT);
     *   SetWindowText(GetHWnd(), "车辆性能仿真");
     *   LOGFONT f; gettextstyle(&f); ... settextstyle(&f);
     *   BeginBatchDraw();
     */
}

void ui_close(void)
{
    /* TODO: closegraph(); */
}

void ui_begin_frame(void)
{
    /* TODO: cleardevice(); */
}

void ui_end_frame(void)
{
    /* TODO: FlushBatchDraw(); Sleep(20); */
}

int ui_main_menu(void)
{
    /* TODO: 第三阶段实现菜单绘制与按键响应
     * 当前直接返回 0（退出）以便骨架阶段可编译运行
     */
    return 0;
}

void ui_draw_speedometer(float speed_kmh, float rpm, int cx, int cy, int radius)
{
    /* TODO: 绘制仪表盘圆弧、刻度、指针 */
    (void)speed_kmh; (void)rpm; (void)cx; (void)cy; (void)radius;
}

void ui_draw_car_side(int x, int y, COLORREF color)
{
    /* TODO: 矩形车体 + 两个圆形车轮 */
    (void)x; (void)y; (void)color;
}

void ui_draw_track(int x, int y, int w, int h)
{
    /* TODO: 灰色地面 + 白色虚线中线 */
    (void)x; (void)y; (void)w; (void)h;
}

void ui_draw_vt_curve(float v_arr[], int count, float dt, COLORREF color,
                      int ox, int oy, int w, int h)
{
    /* TODO: 坐标轴 + 折线绘制 */
    (void)v_arr; (void)count; (void)dt; (void)color;
    (void)ox; (void)oy; (void)w; (void)h;
}

void ui_draw_st_curve(float s_arr[], int count, float dt, COLORREF color,
                      int ox, int oy, int w, int h)
{
    /* TODO */
    (void)s_arr; (void)count; (void)dt; (void)color;
    (void)ox; (void)oy; (void)w; (void)h;
}

void ui_draw_comparison_legend(Car cars[], int count, COLORREF colors[])
{
    /* TODO */
    (void)cars; (void)count; (void)colors;
}

void ui_draw_comparison_table(Car cars[], float t100[], float brake_dist[],
                              float vmax[], int count)
{
    /* TODO */
    (void)cars; (void)t100; (void)brake_dist; (void)vmax; (void)count;
}

int ui_select_car(Car cars[], int count)
{
    /* TODO: 第三阶段实现滚动列表，当前返回第 0 辆 */
    (void)cars; (void)count;
    return 0;
}

int ui_input_car(Car *out)
{
    /* TODO: 第三阶段实现逐字段输入面板 */
    (void)out;
    return 0;
}
