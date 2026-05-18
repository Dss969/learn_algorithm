/*
 * ui.h  —  EasyX 图形界面接口
 *
 * 依赖：EasyX 图形库（https://easyx.cn）
 * 目标平台：Windows（EasyX 仅支持 Windows）
 */
#ifndef UI_H
#define UI_H

#include "car.h"
#include <graphics.h>   /* EasyX 核心头文件，安装 EasyX 后可用 */

/* 窗口分辨率 */
#define WIN_WIDTH   1280
#define WIN_HEIGHT   720

/* 多车对比配色（最多支持 3 辆同时对比） */
extern COLORREF g_car_colors[3];

/* ---- 生命周期 ---- */

/* 创建 EasyX 窗口，设置中文字体，开启批量绘图模式 */
void ui_init(void);

/* 关闭 EasyX 窗口 */
void ui_close(void);

/* ---- 帧控制 ---- */

/* 每帧开始：cleardevice() 清屏 */
void ui_begin_frame(void);

/* 每帧结束：FlushBatchDraw() + Sleep(20ms) 控制约 50 fps */
void ui_end_frame(void);

/* ---- 主菜单 ----
 * 绘制全屏菜单背景和五个选项，等待键盘输入。
 * 返回值：1=加速测试  2=制动测试  3=车型对比  4=车型管理  0=退出
 */
int ui_main_menu(void);

/* ---- 仪表盘 ----
 * 在屏幕坐标 (cx, cy) 处绘制半径 radius 的圆形仪表盘。
 *   speed_kmh —— 当前车速 km/h（0~260），控制指针角度
 *   rpm       —— 估算转速（显示用），控制转速表指针
 */
void ui_draw_speedometer(float speed_kmh, float rpm, int cx, int cy, int radius);

/* ---- 简笔侧视车 ----
 * 在 (x, y) 处绘制宽约 160px 的矩形车体 + 圆形车轮，先占位，后期可换位图。
 */
void ui_draw_car_side(int x, int y, COLORREF color);

/* ---- 跑道背景 ----
 * 在矩形区域 (x, y, w, h) 内绘制简单跑道（灰色地面 + 白色虚线）。
 */
void ui_draw_track(int x, int y, int w, int h);

/* ---- v-t 曲线 ----
 * 在区域 (ox, oy) 为左下角、宽 w 高 h 的矩形内绘制速度-时间曲线。
 *   v_arr[] —— 速度序列（m/s）
 *   count   —— 有效数据点数
 *   dt      —— 时间步长（s），用于计算横轴刻度
 *   color   —— 曲线颜色
 */
void ui_draw_vt_curve(float v_arr[], int count, float dt, COLORREF color,
                      int ox, int oy, int w, int h);

/* ---- s-t 曲线（制动测试专用） ---- */
void ui_draw_st_curve(float s_arr[], int count, float dt, COLORREF color,
                      int ox, int oy, int w, int h);

/* ---- 对比模块 ---- */

/* 在图表右侧绘制彩色图例（车型名 + 色块） */
void ui_draw_comparison_legend(Car cars[], int count, COLORREF colors[]);

/* 在屏幕下方绘制对比表格：破百时间、制动距离、理论最高车速 */
void ui_draw_comparison_table(Car cars[], float t100[], float brake_dist[],
                              float vmax[], int count);

/* ---- 车型选择面板 ----
 * 显示滚动列表，↑↓ 移动高亮，Enter 确认，Esc 取消。
 * 返回选中的数组索引；取消返回 -1。
 */
int ui_select_car(Car cars[], int count);

/* ---- 参数录入面板（车型管理用）----
 * 在屏幕上逐字段显示输入框，键盘输入数值，Enter 逐字段推进，Esc 取消。
 * 返回 1=确认录入  0=取消
 */
int ui_input_car(Car *out);

#endif /* UI_H */
