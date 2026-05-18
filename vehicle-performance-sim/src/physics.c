/*
 * physics.c  —  物理公式实现
 *
 * 【第二阶段实现，当前为函数存根】
 * 下一提交将逐一填充每个公式，并附详细推导注释。
 */
#include "physics.h"

/*
 * 驱动力：Ft = T * i / r
 * 单位：N·m * (无量纲) / m = N   ✓
 */
float physics_drive_force(float torque_nm, float gear_ratio, float wheel_radius_m)
{
    /* TODO: 第二阶段实现 */
    (void)torque_nm; (void)gear_ratio; (void)wheel_radius_m;
    return 0.0f;
}

/*
 * 空气阻力：Fw = 0.5 * AIR_DENSITY * Cd * A * v²
 */
float physics_air_resistance(float drag_coeff, float frontal_area_m2, float speed_ms)
{
    /* TODO: 第二阶段实现 */
    (void)drag_coeff; (void)frontal_area_m2; (void)speed_ms;
    return 0.0f;
}

/*
 * 滚动阻力：Ff = ROLLING_COEFF * mass_kg * GRAVITY
 */
float physics_rolling_resistance(float mass_kg)
{
    /* TODO: 第二阶段实现 */
    (void)mass_kg;
    return 0.0f;
}

/*
 * 净加速度：a = (Ft - Fw - Ff) / m
 */
float physics_acceleration(Car *car, float speed_ms)
{
    /* TODO: 第二阶段实现 */
    (void)car; (void)speed_ms;
    return 0.0f;
}

/*
 * 制动距离：s = v² / (2 * a_brake)
 */
float physics_brake_distance(float speed_ms, float max_brake_decel)
{
    /* TODO: 第二阶段实现 */
    (void)speed_ms; (void)max_brake_decel;
    return 0.0f;
}

/*
 * 破百时间估算：dt 步进累计，直到 v >= TARGET_SPEED
 */
float physics_zero_to_hundred(Car *car)
{
    /* TODO: 第二阶段实现 */
    (void)car;
    return -1.0f;
}
