/*
 * physics.h  —  汽车理论物理公式接口
 *
 * 公式来源：《汽车理论》第 5 版（余志生著）
 * 简化假设：
 *   - 电机 / 发动机始终输出最大扭矩（忽略特性曲线，适合教学）
 *   - 路面水平（坡度阻力 = 0）
 *   - 忽略传动系统效率损失（η = 1）
 */
#ifndef PHYSICS_H
#define PHYSICS_H

#include "car.h"

/* ---------- 物理常数 ---------- */
#define AIR_DENSITY    1.225f   /* 空气密度 kg/m³（标准大气，20°C，海平面） */
#define ROLLING_COEFF  0.015f   /* 滚动阻力系数（普通轿车 × 沥青路面） */
#define GRAVITY        9.81f    /* 重力加速度 m/s² */
#define DT             0.02f    /* 仿真时间步长 s（对应约 50 fps） */
#define TARGET_SPEED   27.78f   /* 100 km/h 换算为 m/s（≈ 100/3.6） */
#define SIM_MAX_TIME   30.0f    /* 超过此秒数仍未破百视为失败 */

/*
 * 驱动力（N）
 * 公式：Ft = T * i / r
 *   T  —— 最大扭矩 N·m
 *   i  —— 综合传动比（变速箱档位传动比 × 主减速比）
 *   r  —— 轮胎滚动半径 m
 */
float physics_drive_force(float torque_nm, float gear_ratio, float wheel_radius_m);

/*
 * 空气阻力（N）
 * 公式：Fw = 0.5 * ρ * Cd * A * v²
 *   ρ  —— 空气密度 1.225 kg/m³
 *   Cd —— 风阻系数
 *   A  —— 迎风面积 m²
 *   v  —— 当前车速 m/s
 */
float physics_air_resistance(float drag_coeff, float frontal_area_m2, float speed_ms);

/*
 * 滚动阻力（N）
 * 公式：Ff = f * m * g
 *   f  —— 滚动阻力系数 0.015
 *   m  —— 整备质量 kg
 *   g  —— 9.81 m/s²
 */
float physics_rolling_resistance(float mass_kg);

/*
 * 净加速度（m/s²）
 * 公式：a = (Ft − Fw − Ff) / m
 * 注：当 a < 0（阻力大于驱动力）时返回 0，车辆不会倒退
 */
float physics_acceleration(Car *car, float speed_ms);

/*
 * 制动距离（m，理论值）
 * 公式：s = v² / (2 * a_brake)
 * 注：忽略驾驶员反应时间（教学简化）
 */
float physics_brake_distance(float speed_ms, float max_brake_decel);

/*
 * 估算 0-100 km/h 破百时间（s）
 * 内部跑一遍 dt 步进仿真，返回累计时间
 * 若 SIM_MAX_TIME 内未破百，返回 -1
 */
float physics_zero_to_hundred(Car *car);

#endif /* PHYSICS_H */
