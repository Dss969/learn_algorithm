/*
 * simulation.h  —  仿真主流程接口
 *
 * 分为两层：
 *   下层：sim_accel / sim_brake  —— 纯物理计算，不涉及图形
 *   上层：sim_run_*              —— 含 EasyX 动画的完整交互流程
 */
#ifndef SIMULATION_H
#define SIMULATION_H

#include "car.h"
#include "physics.h"

/* 仿真最大步数：50 fps × 100 s = 5000 步 */
#define SIM_MAX_STEPS  5000

/* ---- 加速仿真结果 ---- */
typedef struct {
    float v_arr[SIM_MAX_STEPS];  /* 每个时间步的速度（m/s） */
    float t100;                  /* 0-100 km/h 所需时间（s），-1 = 未破百 */
    int   steps;                 /* 实际仿真步数 */
} AccelResult;

/* ---- 制动仿真结果 ---- */
typedef struct {
    float s_arr[SIM_MAX_STEPS];  /* 每个时间步的累计位移（m） */
    float total_dist;            /* 总制动距离（m） */
    float total_time;            /* 总制动时间（s） */
    int   steps;                 /* 实际仿真步数 */
} BrakeResult;

/* ---- 纯物理仿真（无 UI） ---- */

/*
 * 加速仿真：从 v=0 以 DT 步进，直到破百或超过 SIM_MAX_TIME。
 * 填充 AccelResult，t100=-1 表示 30s 内未达到 100 km/h。
 */
void sim_accel(Car *car, AccelResult *out);

/*
 * 制动仿真：从 TARGET_SPEED (100 km/h) 开始，以 max_brake_decel 减速到 0。
 * 填充 BrakeResult。
 */
void sim_brake(Car *car, BrakeResult *out);

/* ---- 带 EasyX 动画的完整交互流程 ---- */

/* 加速测试：选车 → 仿真 → 仪表盘/跑道动画 → 破百后显示 v-t 曲线 */
void sim_run_accel(Car cars[], int car_count);

/* 制动测试：选车 → 显示初速 → 空格触发 → 动画 → 显示制动距离 + s-t 曲线 */
void sim_run_brake(Car cars[], int car_count);

/* 多车对比：选 2-3 辆 → 批量仿真 → 叠加 v-t 曲线 + 参数对比表格 */
void sim_run_comparison(Car cars[], int car_count);

/* 车型管理：键盘录入新车 / 选车删除 */
void sim_run_car_manager(Car cars[], int *car_count);

#endif /* SIMULATION_H */
