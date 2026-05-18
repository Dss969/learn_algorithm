/*
 * ui.h  —  EasyX 图形界面接口
 */
#ifndef UI_H
#define UI_H

#include "car.h"
#include <graphics.h>

#define WIN_WIDTH   1280
#define WIN_HEIGHT   720

extern COLORREF g_car_colors[3];

void ui_init(void);
void ui_close(void);
void ui_begin_frame(void);
void ui_end_frame(void);

/* 返回值：1-6 对应功能，0=退出 */
int ui_main_menu(void);

void ui_draw_speedometer(float speed_kmh, float rpm, int cx, int cy, int radius);
void ui_draw_car_side(int x, int y, COLORREF color);

/* dash_offset：路面虚线滚动偏移像素，静止画面传 0 */
void ui_draw_track(int x, int y, int w, int h, int dash_offset);

/*
 * max_v_ms_hint > 0：直接使用该值作 Y 轴上限（多曲线共享同一坐标系）。
 * max_v_ms_hint = 0：自动缩放到数据最大值，向上取整到 20 km/h 倍数。
 */
void ui_draw_vt_curve(float v_arr[], int count, float dt, COLORREF color,
                      float max_v_ms_hint,
                      int ox, int oy, int w, int h);

void ui_draw_st_curve(float s_arr[], int count, float dt, COLORREF color,
                      int ox, int oy, int w, int h);

void ui_draw_comparison_legend(Car cars[], int count, COLORREF colors[]);
void ui_draw_comparison_table(Car cars[], float t100[], float brake_dist[],
                              float vmax[], int count);

int ui_select_car(Car cars[], int count);
int ui_input_car(Car *out);

#endif
