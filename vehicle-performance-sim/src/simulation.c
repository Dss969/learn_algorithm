/*
 * simulation.c  —  仿真流程实现
 *
 * 【第四阶段实现，当前为函数存根】
 * 依赖 physics.c（第二阶段）和 ui.c（第三阶段）完成后联调。
 */
#include "simulation.h"
#include "ui.h"
#include <string.h>

/* ------------------------------------------------------------------ */
/* 加速仿真（纯物理，无 UI）                                             */
/* ------------------------------------------------------------------ */
void sim_accel(Car *car, AccelResult *out)
{
    /* TODO: 第四阶段实现
     * 伪代码：
     *   v = 0, t = 0, step = 0
     *   while (v < TARGET_SPEED && t < SIM_MAX_TIME && step < SIM_MAX_STEPS):
     *       a = physics_acceleration(car, v)
     *       v += a * DT
     *       t += DT
     *       out->v_arr[step++] = v
     *       if v >= TARGET_SPEED: out->t100 = t
     */
    memset(out, 0, sizeof(AccelResult));
    out->t100 = -1.0f;
}

/* ------------------------------------------------------------------ */
/* 制动仿真（纯物理，无 UI）                                             */
/* ------------------------------------------------------------------ */
void sim_brake(Car *car, BrakeResult *out)
{
    /* TODO: 第四阶段实现
     * 伪代码：
     *   v = TARGET_SPEED, s = 0, step = 0
     *   while (v > 0 && step < SIM_MAX_STEPS):
     *       v -= car->max_brake_decel * DT
     *       if v < 0: v = 0
     *       s += v * DT
     *       out->s_arr[step++] = s
     *   out->total_dist = s
     */
    memset(out, 0, sizeof(BrakeResult));
    (void)car;
}

/* ------------------------------------------------------------------ */
/* 加速测试完整流程（含 EasyX 动画）                                     */
/* ------------------------------------------------------------------ */
void sim_run_accel(Car cars[], int car_count)
{
    /* TODO: 第四阶段实现
     * 流程：
     *   1. idx = ui_select_car(cars, car_count)
     *   2. sim_accel(&cars[idx], &result)
     *   3. 帧循环重放 v_arr[]：
     *        ui_begin_frame()
     *        ui_draw_track(...)  ui_draw_car_side(x, ...)
     *        ui_draw_speedometer(v*3.6, rpm, ...)
     *        ui_end_frame()
     *   4. 曲线：ui_draw_vt_curve(...)
     */
    (void)cars; (void)car_count;
}

/* ------------------------------------------------------------------ */
/* 制动测试完整流程                                                      */
/* ------------------------------------------------------------------ */
void sim_run_brake(Car cars[], int car_count)
{
    /* TODO */
    (void)cars; (void)car_count;
}

/* ------------------------------------------------------------------ */
/* 多车对比完整流程                                                      */
/* ------------------------------------------------------------------ */
void sim_run_comparison(Car cars[], int car_count)
{
    /* TODO:
     * 1. 循环 ui_select_car() 选 2-3 辆
     * 2. 对每辆跑 sim_accel()、sim_brake()
     * 3. 用 g_car_colors[] 叠加绘制 v-t 曲线
     * 4. ui_draw_comparison_legend() + ui_draw_comparison_table()
     */
    (void)cars; (void)car_count;
}

/* ------------------------------------------------------------------ */
/* 车型管理流程                                                          */
/* ------------------------------------------------------------------ */
void sim_run_car_manager(Car cars[], int *car_count)
{
    /* TODO:
     * 菜单：a=添加  d=删除  Esc=返回
     * 添加：ui_input_car() → car_add()
     * 删除：ui_select_car() → car_delete()
     */
    (void)cars; (void)car_count;
}
