/*
 * simulation.c  —  仿真主流程实现
 *
 * 分两层：
 *   下层 sim_accel / sim_brake  —  纯物理 dt 步进，不涉及图形
 *   上层 sim_run_*              —  调用 physics + ui 做完整演示
 */
#include "simulation.h"
#include "physics.h"
#include "ui.h"
#include <stdio.h>
#include <string.h>
#include <conio.h>    /* _getch() */
#include <windows.h>  /* Sleep() */

/* ============================================================ */
/* 内部辅助：屏幕任意位置显示提示文字 + 等待按键 */
/* ============================================================ */
static void wait_any_key(const char *msg)
{
    settextstyle(18, 0, "宋体");
    settextcolor(RGB(120, 120, 145));
    outtextxy(50, WIN_HEIGHT - 36, msg ? msg : "按任意键返回主菜单...");
    FlushBatchDraw();
    _getch();
}

/* ============================================================ */
/* 加速仿真（纯物理，无 UI）                                   */
/* ============================================================ */
void sim_accel(Car *car, AccelResult *out)
{
    float v = 0.0f;   /* 当前车速 m/s */
    float t = 0.0f;   /* 累计时间 s */
    float a;

    memset(out, 0, sizeof(AccelResult));
    out->t100 = -1.0f;

    while (t < SIM_MAX_TIME && out->steps < SIM_MAX_STEPS) {
        a = physics_acceleration(car, v);

        /* 已到极限车速（驱动力 ≤ 阻力），进入匹速巡航 */
        if (a <= 0.0f) {
            /* 不记录更多点，直接结束 */
            break;
        }

        /* 速度积分：v(t+dt) = v(t) + a·dt */
        v += a * DT;
        t += DT;

        out->v_arr[out->steps++] = v;

        /* 记录破百时刻 */
        if (out->t100 < 0.0f && v >= TARGET_SPEED)
            out->t100 = t;
    }
}

/* ============================================================ */
/* 制动仿真（纯物理，无 UI）                                   */
/* ============================================================ */
void sim_brake(Car *car, BrakeResult *out)
{
    float v = TARGET_SPEED;   /* 初始车速 100 km/h = 27.78 m/s */
    float s = 0.0f;           /* 累计位移 */
    float t = 0.0f;

    memset(out, 0, sizeof(BrakeResult));

    /*
     * 制动动学：v(t+dt) = v(t) − a_brake·dt
     * 位移积分：s(t+dt) = s(t) + v(t+dt)·dt
     */
    while (v > 0.0f && out->steps < SIM_MAX_STEPS) {
        v -= car->max_brake_decel * DT;
        if (v < 0.0f) v = 0.0f;

        s += v * DT;
        t += DT;
        out->s_arr[out->steps++] = s;
    }

    out->total_dist = s;
    out->total_time = t;
}

/* ============================================================ */
/* 加速测试完整流程                                             */
/* ============================================================ */
void sim_run_accel(Car cars[], int car_count)
{
    AccelResult result;
    int   idx, step;
    float v_kmh, rpm;
    float dist = 0.0f;   /* 跑过的路程 m */
    char  buf[64];

    /* 跑道区域（屏幕右半底部） */
    int   trk_x = 520, trk_y = 340, trk_w = 720, trk_h = 280;
    /* 车在跑道上的 Y 坐标（路面中齐） */
    int   car_y  = trk_y + trk_h / 2 - 45;
    /* 屏幕每像素代表的路程（m），100m 映射到跑道宽度 */
    float px_per_m = (float)trk_w / 100.0f;

    /* 选车 */
    idx = ui_select_car(cars, car_count);
    if (idx < 0) return;

    /* 运行物理仿真 */
    sim_accel(&cars[idx], &result);

    /* ---- 帧循环重放动画 ---- */
    for (step = 0; step < result.steps; step++) {
        v_kmh = result.v_arr[step] * 3.6f;

        /* 估算转速：轮轴转速 × 传动比 */
        rpm = (result.v_arr[step] / cars[idx].wheel_radius_m)
              * cars[idx].gear_ratio * 60.0f / (2.0f * 3.14159f);
        if (rpm > 8000.0f) rpm = 8000.0f;

        /* 累计位移（循环显示：超过跑道宽度则回绕） */
        dist += result.v_arr[step] * DT;
        while (dist > 100.0f) dist -= 100.0f;

        ui_begin_frame();

        /* 左侧：仪表盘 */
        ui_draw_speedometer(v_kmh, rpm, 240, 300, 190);

        /* 左上：车型名 + 时间 */
        settextstyle(20, 0, "宋体");
        settextcolor(RGB(200, 200, 200));
        outtextxy(30, 30, cars[idx].name);

        settextstyle(18, 0, "宋体");
        settextcolor(RGB(150, 150, 170));
        snprintf(buf, sizeof(buf), "t = %.2f s", step * DT);
        outtextxy(30, 58, buf);

        /* 左居中下方：实时参数 */
        settextstyle(17, 0, "宋体");
        settextcolor(RGB(120, 200, 120));
        snprintf(buf, sizeof(buf), "车速: %.1f km/h", v_kmh);
        outtextxy(30, WIN_HEIGHT - 100, buf);
        snprintf(buf, sizeof(buf), "加速度: %.2f m/s2",
                 physics_acceleration(&cars[idx], result.v_arr[step]));
        outtextxy(30, WIN_HEIGHT - 76, buf);

        /* 右侧：跑道 + 车辆 */
        ui_draw_track(trk_x, trk_y, trk_w, trk_h);
        ui_draw_car_side(trk_x + (int)(dist * px_per_m),
                         car_y, g_car_colors[0]);

        /* 破百后显示标志 */
        if (result.t100 > 0.0f && step * DT >= result.t100) {
            settextstyle(28, 0, "宋体");
            settextcolor(RGB(255, 220, 50));
            snprintf(buf, sizeof(buf), "0-100: %.2f s", result.t100);
            outtextxy(trk_x + 20, trk_y + 20, buf);
        }

        ui_end_frame();
    }

    /* ---- 展示 v-t 曲线 ---- */
    ui_begin_frame();

    settextstyle(24, 0, "宋体");
    settextcolor(RGB(255, 220, 50));
    outtextxy(50, 20, "加速测试结果 — v-t 曲线");

    settextstyle(20, 0, "宋体");
    settextcolor(g_car_colors[0]);
    outtextxy(50, 54, cars[idx].name);

    if (result.t100 > 0.0f) {
        settextcolor(RGB(100, 255, 100));
        snprintf(buf, sizeof(buf), "0-100 km/h: %.2f s", result.t100);
        outtextxy(50, 80, buf);
    } else {
        settextcolor(RGB(255, 100, 100));
        outtextxy(50, 80, "30s 内未破百");
    }

    ui_draw_vt_curve(result.v_arr, result.steps, DT,
                     g_car_colors[0], 100, WIN_HEIGHT - 80,
                     WIN_WIDTH - 200, WIN_HEIGHT - 180);

    wait_any_key(NULL);
}

/* ============================================================ */
/* 制动测试完整流程                                             */
/* ============================================================ */
void sim_run_brake(Car cars[], int car_count)
{
    BrakeResult result;
    int   idx, step;
    float v_kmh;
    float brake_theoretical;
    char  buf[64];

    int trk_x = 480, trk_y = 360, trk_w = 760, trk_h = 260;
    int car_y  = trk_y + trk_h / 2 - 45;
    float px_per_m;

    idx = ui_select_car(cars, car_count);
    if (idx < 0) return;

    /* --- 展示初始车速，等待空格键触发制动 --- */
    ui_begin_frame();
    ui_draw_speedometer(100.0f, 0.0f, 240, 300, 190);
    settextstyle(22, 0, "宋体");
    settextcolor(RGB(255, 180, 50));
    outtextxy(30, 30, cars[idx].name);
    outtextxy(30, 60, "制动测试 — 初始车速: 100 km/h");
    settextcolor(RGB(120, 120, 145));
    outtextxy(30, WIN_HEIGHT - 50, "按 【空格】 开始制动");
    ui_draw_track(trk_x, trk_y, trk_w, trk_h);
    ui_draw_car_side(trk_x + 20, car_y, g_car_colors[1]);
    wait_any_key("按空格开始制动...");

    /* --- 运行制动仿真 --- */
    sim_brake(&cars[idx], &result);

    /* px_per_m 将总制动距离映射到 80% 跑道宽度 */
    px_per_m = (result.total_dist > 0.0f)
               ? (float)(trk_w * 8 / 10) / result.total_dist
               : 3.0f;

    for (step = 0; step < result.steps; step++) {
        v_kmh = (step > 0)
                ? (result.s_arr[step] - result.s_arr[step-1]) / DT * 3.6f
                : TARGET_SPEED * 3.6f;
        if (v_kmh < 0.0f) v_kmh = 0.0f;

        ui_begin_frame();
        ui_draw_speedometer(v_kmh, 0.0f, 240, 300, 190);

        settextstyle(20, 0, "宋体");
        settextcolor(RGB(200, 200, 200));
        outtextxy(30, 30, cars[idx].name);
        snprintf(buf, sizeof(buf), "制动距离: %.1f m", result.s_arr[step]);
        settextcolor(RGB(100, 200, 255));
        outtextxy(30, 58, buf);

        ui_draw_track(trk_x, trk_y, trk_w, trk_h);
        ui_draw_car_side(trk_x + (int)(result.s_arr[step] * px_per_m),
                         car_y, g_car_colors[1]);
        ui_end_frame();
    }

    /* --- 结果页 --- */
    brake_theoretical = physics_brake_distance(TARGET_SPEED,
                                               cars[idx].max_brake_decel);
    ui_begin_frame();
    settextstyle(24, 0, "宋体");
    settextcolor(RGB(255, 220, 50));
    outtextxy(50, 20, "制动测试结果 — s-t 曲线");

    settextstyle(20, 0, "宋体");
    settextcolor(g_car_colors[1]);
    outtextxy(50, 54, cars[idx].name);

    settextcolor(RGB(100, 255, 100));
    snprintf(buf, sizeof(buf), "仿真制动距离: %.1f m", result.total_dist);
    outtextxy(50, 80, buf);
    settextcolor(RGB(180, 180, 200));
    snprintf(buf, sizeof(buf), "理论制动距离: %.1f m", brake_theoretical);
    outtextxy(50, 106, buf);
    snprintf(buf, sizeof(buf), "制动时间: %.2f s", result.total_time);
    outtextxy(50, 132, buf);

    ui_draw_st_curve(result.s_arr, result.steps, DT,
                     g_car_colors[1], 100, WIN_HEIGHT - 80,
                     WIN_WIDTH - 200, WIN_HEIGHT - 180);
    wait_any_key(NULL);
}

/* ============================================================ */
/* 多车对比完整流程                                             */
/* ============================================================ */
void sim_run_comparison(Car cars[], int car_count)
{
    AccelResult accel[3];
    BrakeResult brake[3];
    Car         sel_cars[3];
    float       t100[3], brake_dist[3], vmax[3];
    int         sel_idx[3];
    int         n, i;
    char        buf[64];

    /* --- 选择 2-3 车 --- */
    ui_begin_frame();
    settextstyle(22, 0, "宋体");
    settextcolor(RGB(255, 220, 50));
    outtextxy(WIN_WIDTH/2 - 120, 30, "车型对比：选 2-3 车");
    settextcolor(RGB(120, 120, 145));
    outtextxy(WIN_WIDTH/2 - 200, 62, "Esc 选两车后可直接开始对比");
    ui_end_frame();

    n = 0;
    while (n < 3) {
        int idx = ui_select_car(cars, car_count);
        if (idx < 0 && n >= 2) break;   /* Esc 且已选了两车 */
        if (idx < 0 && n < 2)  continue; /* 不足两车时继续选 */

        sel_idx[n]  = idx;
        sel_cars[n] = cars[idx];
        n++;
    }
    if (n < 2) return;

    /* --- 批量仿真 --- */
    for (i = 0; i < n; i++) {
        sim_accel(&sel_cars[i], &accel[i]);
        sim_brake(&sel_cars[i], &brake[i]);

        t100[i]       = accel[i].t100;
        brake_dist[i] = brake[i].total_dist;
        /* 最高车速：取仿真最后一步的速度 */
        vmax[i] = (accel[i].steps > 0)
                  ? accel[i].v_arr[accel[i].steps - 1]
                  : 0.0f;
    }

    /* --- 绘制叠加 v-t 曲线 --- */
    ui_begin_frame();

    settextstyle(22, 0, "宋体");
    settextcolor(RGB(255, 220, 50));
    outtextxy(50, 18, "车型对比 — v-t 曲线");

    for (i = 0; i < n; i++) {
        /* 每辆车用不同颜色叠在同一层 */
        ui_draw_vt_curve(accel[i].v_arr, accel[i].steps, DT,
                         g_car_colors[i],
                         90, WIN_HEIGHT - 210,
                         WIN_WIDTH - 300, WIN_HEIGHT - 330);
    }

    ui_draw_comparison_legend(sel_cars, n, g_car_colors);
    ui_draw_comparison_table(sel_cars, t100, brake_dist, vmax, n);

    /* 简明标题提示 */
    settextstyle(17, 0, "宋体");
    for (i = 0; i < n; i++) {
        settextcolor(g_car_colors[i]);
        if (t100[i] < 0.0f)
            snprintf(buf, sizeof(buf), "%s: >30s", sel_cars[i].name);
        else
            snprintf(buf, sizeof(buf), "%s: %.2fs", sel_cars[i].name, t100[i]);
        outtextxy(50, 48 + i * 24, buf);
    }

    wait_any_key(NULL);
}

/* ============================================================ */
/* 车型管理完整流程                                             */
/* ============================================================ */
void sim_run_car_manager(Car cars[], int *car_count)
{
    int  ch, del_idx;
    char buf[128];

    while (1) {
        int i;

        /* 绘制菜单 */
        ui_begin_frame();

        settextstyle(26, 0, "宋体");
        settextcolor(RGB(255, 220, 50));
        outtextxy(50, 30, "车型管理");

        settextstyle(20, 0, "宋体");
        settextcolor(RGB(100, 200, 255));
        outtextxy(50, 72, "[A]  添加新车型");
        settextcolor(RGB(255, 130, 100));
        outtextxy(50, 100, "[D]  删除车型");
        settextcolor(RGB(180, 180, 180));
        outtextxy(50, 128, "[Esc] 返回主菜单");

        /* 车型列表（只读） */
        settextstyle(18, 0, "宋体");
        settextcolor(RGB(150, 150, 170));
        outtextxy(50, 166, "当前车型库：");

        for (i = 0; i < *car_count; i++) {
            settextcolor(RGB(200, 200, 200));
            snprintf(buf, sizeof(buf),
                     "[%d] %-22s  %.0f kW  %.0f N.m  %.0f kg",
                     i, cars[i].name,
                     cars[i].power_kw,
                     cars[i].torque_nm,
                     cars[i].mass_kg);
            outtextxy(70, 192 + i * 36, buf);
        }

        snprintf(buf, sizeof(buf), "共 %d / %d 车型", *car_count, MAX_CARS);
        settextstyle(15, 0, "宋体");
        settextcolor(RGB(90, 90, 110));
        outtextxy(50, WIN_HEIGHT - 36, buf);

        ui_end_frame();

        ch = _getch();

        if (ch == 27) {
            /* Esc 返回 */
            break;

        } else if (ch == 'a' || ch == 'A') {
            /* 添加新车型 */
            Car new_car;
            if (*car_count >= MAX_CARS) {
                ui_begin_frame();
                settextstyle(22, 0, "宋体");
                settextcolor(RGB(255, 100, 100));
                outtextxy(WIN_WIDTH/2 - 120, WIN_HEIGHT/2, "车型库已满，无法添加");
                wait_any_key(NULL);
                continue;
            }
            if (ui_input_car(&new_car)) {
                car_add(cars, car_count, new_car);
            }

        } else if (ch == 'd' || ch == 'D') {
            /* 删除车型 */
            if (*car_count == 0) continue;

            del_idx = ui_select_car(cars, *car_count);
            if (del_idx >= 0) {
                /* 二次确认 */
                ui_begin_frame();
                settextstyle(22, 0, "宋体");
                settextcolor(RGB(255, 180, 50));
                snprintf(buf, sizeof(buf),
                         "确认删除 [%s] ?   Enter=确认   其它键=取消",
                         cars[del_idx].name);
                outtextxy(50, WIN_HEIGHT / 2, buf);
                FlushBatchDraw();

                if (_getch() == 13)
                    car_delete(cars, car_count, del_idx);
            }
        }
        /* 其它按键忽略，重绘菜单 */
    }
}
