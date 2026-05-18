/*
 * simulation.h  —  仿真主流程接口
 */
#ifndef SIMULATION_H
#define SIMULATION_H

#include "car.h"
#include "physics.h"

#define SIM_MAX_STEPS  5000

typedef struct {
    float v_arr[SIM_MAX_STEPS];
    float t100;    /* -1 = 未破百 */
    int   steps;
} AccelResult;

typedef struct {
    float s_arr[SIM_MAX_STEPS];
    float total_dist;
    float total_time;
    int   steps;
} BrakeResult;

/* 纯物理仿真（无 UI） */
void sim_accel(Car *car, AccelResult *out);
void sim_brake(Car *car, BrakeResult *out);

/* 带 EasyX 动画的完整交互流程 */
void sim_run_accel(Car cars[], int car_count);
void sim_run_brake(Car cars[], int car_count);
void sim_run_comparison(Car cars[], int car_count);
void sim_run_car_manager(Car cars[], int *car_count);

/* 全库排行榜：批量仿真所有车型，按 0-100 时间排序，绘制横向条形图 */
void sim_run_leaderboard(Car cars[], int car_count);

/* 外特性曲线：选定车型的 Ft(v) 驱动力 vs Fr(v) 行驶阻力图，标注极速和拐点 */
void sim_run_ft_curve(Car cars[], int car_count);

#endif
