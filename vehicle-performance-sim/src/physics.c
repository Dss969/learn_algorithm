/*
 * physics.c  —  汽车理论物理公式实现
 *
 * 公式来源：《汽车理论》第 5 版（余志生著）
 * 简化假设：驱动扭矩取定标最大值，路面水平，传动效率 η=1
 */
#include "physics.h"

/* ------------------------------------------------------------------ */
/*  驱动力（N）                                                          */
/*  Ft = T × i / r                                                      */
/*      T —— 最大扭矩 N·m                                               */
/*      i —— 综合传动比（变速箱 × 主减速比）                            */
/*      r —— 轮胎滚动半径 m                                            */
/*  单位检验：N·m × (无量纲) / m = N  ✓                              */
/* ------------------------------------------------------------------ */
float physics_drive_force(float torque_nm, float gear_ratio, float wheel_radius_m)
{
    return torque_nm * gear_ratio / wheel_radius_m;
}

/* ------------------------------------------------------------------ */
/*  空气阻力（N）                                                          */
/*  Fw = 0.5 × ρ × Cd × A × v²                                         */
/*      ρ  —— 空气密度 1.225 kg/m³                                      */
/*      Cd —— 风阻系数（车型参数）                                    */
/*      A  —— 迎风面积 m²                                               */
/*      v  —— 车速 m/s                                                   */
/*  在低车速时几乎可忽略，高车速时是限速的主要因素                      */
/* ------------------------------------------------------------------ */
float physics_air_resistance(float drag_coeff, float frontal_area_m2, float speed_ms)
{
    return 0.5f * AIR_DENSITY * drag_coeff * frontal_area_m2 * speed_ms * speed_ms;
}

/* ------------------------------------------------------------------ */
/*  滚动阻力（N）                                                          */
/*  Ff = f × m × g                                                      */
/*      f —— 滚动阻力系数 0.015（普通子午车 × 干燥平坦氥青路）       */
/*      m —— 整备质量 kg                                               */
/*      g —— 9.81 m/s²                                                  */
/*  在整个车速范围内基本恒定，不随车速变化                              */
/* ------------------------------------------------------------------ */
float physics_rolling_resistance(float mass_kg)
{
    return ROLLING_COEFF * mass_kg * GRAVITY;
}

/* ------------------------------------------------------------------ */
/*  净加速度（m/s²）                                                       */
/*  a = (Ft − Fw − Ff) / m                                             */
/*  当 Ft < Fw + Ff 时车辆已达到极限车速，返回 0 防止倒退                    */
/* ------------------------------------------------------------------ */
float physics_acceleration(Car *car, float speed_ms)
{
    float Ft, Fw, Ff, F_net;

    /* 驱动力 */
    Ft = physics_drive_force(car->torque_nm, car->gear_ratio, car->wheel_radius_m);

    /* 空气阻力（随车速平方增长，高速时是主要阻力） */
    Fw = physics_air_resistance(car->drag_coeff, car->frontal_area_m2, speed_ms);

    /* 滚动阻力（基本恒定） */
    Ff = physics_rolling_resistance(car->mass_kg);

    /* 合力 = 驱动力 − 各项阻力 */
    F_net = Ft - Fw - Ff;

    /* 车辆不会倒退：当驱动力 ≤ 阻力之和时，加速度取 0 */
    if (F_net < 0.0f) F_net = 0.0f;

    /* 牛顿第二定律：F = m·a ⇒ a = F/m */
    return F_net / car->mass_kg;
}

/* ------------------------------------------------------------------ */
/*  制动距离（m，理论值）                                                 */
/*  s = v² / (2 × a_brake)                                            */
/*  此公式由运动学基本方程 v² = v₀² − 2·a·s 推导（v=0时）             */
/*  忽略：驾驶员反应时间、车轮滲除 ABS 特性（教学简化）               */
/* ------------------------------------------------------------------ */
float physics_brake_distance(float speed_ms, float max_brake_decel)
{
    /* 防尴0山除 */
    if (max_brake_decel <= 0.0f) return 0.0f;
    return (speed_ms * speed_ms) / (2.0f * max_brake_decel);
}

/* ------------------------------------------------------------------ */
/*  破百时间估算：跑一遍 dt 步进仿真                                  */
/* ------------------------------------------------------------------ */
float physics_zero_to_hundred(Car *car)
{
    float v = 0.0f;   /* 当前车速 m/s */
    float t = 0.0f;   /* 累计时间 s */
    float a;          /* 当前加速度 m/s² */

    while (t < SIM_MAX_TIME) {
        a = physics_acceleration(car, v);

        /* 加速度为 0 且车速未达目标，车辆已加速到极限车速，无法破百 */
        if (a <= 0.0f) return -1.0f;

        v += a * DT;   /* 速度积分：v(t+dt) = v(t) + a·dt */
        t += DT;

        if (v >= TARGET_SPEED) return t;   /* 破百！ */
    }
    return -1.0f;   /* 30 s 内未破百 */
}
