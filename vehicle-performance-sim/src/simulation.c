/*
 * simulation.c  —  仿真主流程实现
 */
#include "simulation.h"
#include "physics.h"
#include "ui.h"
#include <stdio.h>
#include <string.h>
#include <windows.h>

static void wait_any_key(const char *hint)
{
    ExMessage msg;
    settextstyle(18, 0, "宋体");
    settextcolor(RGB(110, 110, 140));
    outtextxy(50, WIN_HEIGHT - 36, hint ? hint : "按任意键返回主菜单...");
    FlushBatchDraw();
    flushmessage(EM_KEY);
    getmessage(&msg, EM_KEY);
}

/* ============================================================ */
void sim_accel(Car *car, AccelResult *out)
{
    float v = 0.0f, t = 0.0f, a;
    memset(out, 0, sizeof(AccelResult));
    out->t100 = -1.0f;
    while (t < SIM_MAX_TIME && out->steps < SIM_MAX_STEPS) {
        a = physics_acceleration(car, v);
        if (a <= 0.0f) break;
        v += a * DT;
        t += DT;
        out->v_arr[out->steps++] = v;
        if (out->t100 < 0.0f && v >= TARGET_SPEED) out->t100 = t;
    }
}

void sim_brake(Car *car, BrakeResult *out)
{
    float v = TARGET_SPEED, s = 0.0f, t = 0.0f;
    memset(out, 0, sizeof(BrakeResult));
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
void sim_run_accel(Car cars[], int car_count)
{
    AccelResult result;
    int   idx, step;
    float v_kmh, rpm;
    char  buf[64];
    int   trk_x = 520, trk_y = 340, trk_w = 720, trk_h = 280;
    int   car_y = trk_y + trk_h / 2 - 45;
    int   car_draw_x = trk_x + 60;
    float dash_offset = 0.0f;

    idx = ui_select_car(cars, car_count);
    if (idx < 0) return;
    sim_accel(&cars[idx], &result);

    for (step = 0; step < result.steps; step++) {
        v_kmh = result.v_arr[step] * 3.6f;
        rpm = (result.v_arr[step] / cars[idx].wheel_radius_m)
              * cars[idx].gear_ratio * 60.0f / (2.0f * 3.14159f);
        if (rpm > 8000.0f) rpm = 8000.0f;
        dash_offset += result.v_arr[step] * DT * 6.0f;

        ui_begin_frame();
        ui_draw_speedometer(v_kmh, rpm, 240, 300, 190);
        settextstyle(20, 0, "宋体"); settextcolor(RGB(200,200,200));
        outtextxy(30, 30, cars[idx].name);
        settextstyle(18, 0, "宋体"); settextcolor(RGB(150,150,170));
        snprintf(buf, sizeof(buf), "t = %.2f s", step * DT);
        outtextxy(30, 58, buf);
        settextstyle(17, 0, "宋体"); settextcolor(RGB(120,200,120));
        snprintf(buf, sizeof(buf), "车速: %.1f km/h", v_kmh);
        outtextxy(30, WIN_HEIGHT - 100, buf);
        snprintf(buf, sizeof(buf), "加速度: %.2f m/s2",
                 physics_acceleration(&cars[idx], result.v_arr[step]));
        outtextxy(30, WIN_HEIGHT - 76, buf);
        ui_draw_track(trk_x, trk_y, trk_w, trk_h, (int)dash_offset);
        ui_draw_car_side(car_draw_x, car_y, g_car_colors[0]);
        if (result.t100 > 0.0f && step * DT >= result.t100) {
            settextstyle(28, 0, "宋体"); settextcolor(RGB(255,220,50));
            snprintf(buf, sizeof(buf), "0-100: %.2f s", result.t100);
            outtextxy(trk_x + 20, trk_y + 20, buf);
        }
        ui_end_frame();
    }

    ui_begin_frame();
    settextstyle(24, 0, "宋体"); settextcolor(RGB(255,220,50));
    outtextxy(50, 20, "加速测试结果 — v-t 曲线");
    settextstyle(20, 0, "宋体"); settextcolor(g_car_colors[0]);
    outtextxy(50, 54, cars[idx].name);
    if (result.t100 > 0.0f) {
        settextcolor(RGB(100,255,100));
        snprintf(buf, sizeof(buf), "0-100 km/h: %.2f s", result.t100);
        outtextxy(50, 80, buf);
    } else {
        settextcolor(RGB(255,100,100));
        outtextxy(50, 80, "30s 内未破百");
    }
    /* max_v_ms_hint=0 → 自动缩放 */
    ui_draw_vt_curve(result.v_arr, result.steps, DT,
                     g_car_colors[0], 0.0f,
                     100, WIN_HEIGHT - 80, WIN_WIDTH - 200, WIN_HEIGHT - 180);
    wait_any_key(NULL);
}

/* ============================================================ */
void sim_run_brake(Car cars[], int car_count)
{
    BrakeResult result;
    int   idx, step;
    float v_kmh, brake_theoretical, px_per_m;
    char  buf[64];
    int   trk_x = 480, trk_y = 360, trk_w = 760, trk_h = 260;
    int   car_y = trk_y + trk_h / 2 - 45;

    idx = ui_select_car(cars, car_count);
    if (idx < 0) return;

    ui_begin_frame();
    ui_draw_speedometer(100.0f, 0.0f, 240, 300, 190);
    settextstyle(22, 0, "宋体"); settextcolor(RGB(255,180,50));
    outtextxy(30, 30, cars[idx].name);
    outtextxy(30, 60, "制动测试 — 初始车速: 100 km/h");
    settextcolor(RGB(120,120,145));
    outtextxy(30, WIN_HEIGHT - 50, "按 【空格】 开始制动");
    ui_draw_track(trk_x, trk_y, trk_w, trk_h, 0);
    ui_draw_car_side(trk_x + 20, car_y, g_car_colors[1]);
    FlushBatchDraw();

    {
        ExMessage msg;
        flushmessage(EM_KEY);
        do { getmessage(&msg, EM_KEY); }
        while (!(msg.message == WM_KEYDOWN && msg.vkcode == VK_SPACE));
    }

    sim_brake(&cars[idx], &result);
    px_per_m = (result.total_dist > 0.0f)
               ? (float)(trk_w * 8 / 10) / result.total_dist : 3.0f;

    for (step = 0; step < result.steps; step++) {
        v_kmh = (step > 0)
                ? (result.s_arr[step] - result.s_arr[step-1]) / DT * 3.6f
                : TARGET_SPEED * 3.6f;
        if (v_kmh < 0.0f) v_kmh = 0.0f;
        ui_begin_frame();
        ui_draw_speedometer(v_kmh, 0.0f, 240, 300, 190);
        settextstyle(20, 0, "宋体"); settextcolor(RGB(200,200,200));
        outtextxy(30, 30, cars[idx].name);
        snprintf(buf, sizeof(buf), "制动距离: %.1f m", result.s_arr[step]);
        settextcolor(RGB(100,200,255));
        outtextxy(30, 58, buf);
        ui_draw_track(trk_x, trk_y, trk_w, trk_h, 0);
        ui_draw_car_side(trk_x + (int)(result.s_arr[step] * px_per_m), car_y, g_car_colors[1]);
        ui_end_frame();
    }

    brake_theoretical = physics_brake_distance(TARGET_SPEED, cars[idx].max_brake_decel);
    ui_begin_frame();
    settextstyle(24, 0, "宋体"); settextcolor(RGB(255,220,50));
    outtextxy(50, 20, "制动测试结果 — s-t 曲线");
    settextstyle(20, 0, "宋体"); settextcolor(g_car_colors[1]);
    outtextxy(50, 54, cars[idx].name);
    settextcolor(RGB(100,255,100));
    snprintf(buf, sizeof(buf), "仿真制动距离: %.1f m", result.total_dist);
    outtextxy(50, 80, buf);
    settextcolor(RGB(180,180,200));
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
void sim_run_comparison(Car cars[], int car_count)
{
    AccelResult accel[3];
    BrakeResult brake[3];
    Car  sel_cars[3];
    float t100[3], brake_dist[3], vmax[3];
    int n = 0, i, k;
    char buf[64];
    float shared_max;
    int ticks;

    ui_begin_frame();
    settextstyle(22, 0, "宋体"); settextcolor(RGB(255,220,50));
    outtextxy(WIN_WIDTH/2-120, 30, "车型对比：选 2-3 车");
    settextcolor(RGB(120,120,145));
    outtextxy(WIN_WIDTH/2-200, 62, "Esc 选两车后可直接开始对比");
    ui_end_frame();

    while (n < 3) {
        int idx = ui_select_car(cars, car_count);
        if (idx < 0 && n >= 2) break;
        if (idx < 0 && n <  2) continue;
        sel_cars[n] = cars[idx];
        n++;
    }
    if (n < 2) return;

    for (i = 0; i < n; i++) {
        sim_accel(&sel_cars[i], &accel[i]);
        sim_brake(&sel_cars[i], &brake[i]);
        t100[i]       = accel[i].t100;
        brake_dist[i] = brake[i].total_dist;
        vmax[i] = (accel[i].steps > 0) ? accel[i].v_arr[accel[i].steps - 1] : 0.0f;
    }

    /*
     * 计算共享 Y 轴上限：取所有曲线最大速度，统一向上取整到 20 km/h 倍数。
     * 若各曲线独立缩放，不同比例叠在同一坐标系会导致对比完全失真。
     */
    shared_max = 0.0f;
    for (i = 0; i < n; i++)
        for (k = 0; k < accel[i].steps; k++)
            if (accel[i].v_arr[k] > shared_max) shared_max = accel[i].v_arr[k];
    ticks = (int)(shared_max * 3.6f / 20.0f) + 1;
    if (ticks < 2) ticks = 2;
    shared_max = (float)ticks * 20.0f / 3.6f;

    ui_begin_frame();
    settextstyle(22, 0, "宋体"); settextcolor(RGB(255,220,50));
    outtextxy(50, 18, "车型对比 — v-t 曲线");

    for (i = 0; i < n; i++)
        ui_draw_vt_curve(accel[i].v_arr, accel[i].steps, DT,
                         g_car_colors[i], shared_max,
                         90, WIN_HEIGHT - 210,
                         WIN_WIDTH - 300, WIN_HEIGHT - 330);

    ui_draw_comparison_legend(sel_cars, n, g_car_colors);
    ui_draw_comparison_table(sel_cars, t100, brake_dist, vmax, n);

    settextstyle(17, 0, "宋体");
    for (i = 0; i < n; i++) {
        settextcolor(g_car_colors[i]);
        if (t100[i] < 0.0f) snprintf(buf, sizeof(buf), "%s: >30s", sel_cars[i].name);
        else                 snprintf(buf, sizeof(buf), "%s: %.2fs", sel_cars[i].name, t100[i]);
        outtextxy(50, 48 + i * 24, buf);
    }
    wait_any_key(NULL);
}

/* ============================================================ */
void sim_run_car_manager(Car cars[], int *car_count)
{
    char buf[128];
    ExMessage msg;

    while (1) {
        int i;
        ui_begin_frame();
        settextstyle(26, 0, "宋体"); settextcolor(RGB(255,220,50));
        outtextxy(50, 30, "车型管理");
        settextstyle(20, 0, "宋体");
        settextcolor(RGB(100,200,255)); outtextxy(50, 72,  "[A]  添加新车型");
        settextcolor(RGB(255,130,100)); outtextxy(50, 100, "[D]  删除车型");
        settextcolor(RGB(180,180,180)); outtextxy(50, 128, "[Esc] 返回主菜单");
        settextstyle(18, 0, "宋体"); settextcolor(RGB(150,150,170));
        outtextxy(50, 166, "当前车型库：");
        for (i = 0; i < *car_count; i++) {
            settextcolor(RGB(200,200,200));
            snprintf(buf, sizeof(buf), "[%d] %-22s  %.0f kW  %.0f N.m  %.0f kg",
                     i, cars[i].name, cars[i].power_kw, cars[i].torque_nm, cars[i].mass_kg);
            outtextxy(70, 192 + i * 36, buf);
        }
        snprintf(buf, sizeof(buf), "共 %d / %d 车型", *car_count, MAX_CARS);
        settextstyle(15, 0, "宋体"); settextcolor(RGB(90,90,110));
        outtextxy(50, WIN_HEIGHT - 36, buf);
        ui_end_frame();

        flushmessage(EM_KEY);
        getmessage(&msg, EM_KEY);
        if (msg.message != WM_KEYDOWN) continue;

        if (msg.vkcode == VK_ESCAPE) break;

        if (msg.vkcode == 'A') {
            Car new_car;
            if (*car_count >= MAX_CARS) {
                ui_begin_frame();
                settextstyle(22, 0, "宋体"); settextcolor(RGB(255,100,100));
                outtextxy(WIN_WIDTH/2-120, WIN_HEIGHT/2, "车型库已满，无法添加");
                wait_any_key(NULL);
                continue;
            }
            if (ui_input_car(&new_car)) car_add(cars, car_count, new_car);
        } else if (msg.vkcode == 'D') {
            int del_idx;
            if (*car_count == 0) continue;
            del_idx = ui_select_car(cars, *car_count);
            if (del_idx >= 0) {
                ExMessage cmsg;
                ui_begin_frame();
                settextstyle(22, 0, "宋体"); settextcolor(RGB(255,180,50));
                snprintf(buf, sizeof(buf), "确认删除 [%s] ?   Enter=确认   其它键=取消",
                         cars[del_idx].name);
                outtextxy(50, WIN_HEIGHT/2, buf);
                FlushBatchDraw();
                flushmessage(EM_KEY);
                getmessage(&cmsg, EM_KEY);
                if (cmsg.message == WM_KEYDOWN && cmsg.vkcode == VK_RETURN)
                    car_delete(cars, car_count, del_idx);
            }
        }
    }
}

/* ============================================================ */
/* 全库排行榜                                                   */
/* ============================================================ */
void sim_run_leaderboard(Car cars[], int car_count)
{
    AccelResult accel;
    BrakeResult brake;
    float  t100[MAX_CARS], vmax[MAX_CARS], bdist[MAX_CARS];
    int    sorted[MAX_CARS];
    int    i, j, show_n;
    char   buf[128];
    float  ref_t;
    int    bar_x = 320, bar_max_w = 700;
    int    bar_y0 = 92, bar_h = 46, bar_gap = 8;

    if (car_count == 0) return;

    for (i = 0; i < car_count; i++) {
        sim_accel(&cars[i], &accel);
        sim_brake(&cars[i], &brake);
        t100[i]  = accel.t100;
        vmax[i]  = (accel.steps > 0) ? accel.v_arr[accel.steps - 1] : 0.0f;
        bdist[i] = brake.total_dist;
        sorted[i] = i;
    }

    /* 按 t100 升序排列，-1（未破百）沉底 */
    for (i = 0; i < car_count - 1; i++) {
        for (j = 0; j < car_count - 1 - i; j++) {
            float ta = (t100[sorted[j]]   < 0.0f) ? 9999.0f : t100[sorted[j]];
            float tb = (t100[sorted[j+1]] < 0.0f) ? 9999.0f : t100[sorted[j+1]];
            if (ta > tb) { int tmp = sorted[j]; sorted[j] = sorted[j+1]; sorted[j+1] = tmp; }
        }
    }

    show_n = (car_count < 10) ? car_count : 10;

    /* 参考时间（最慢一辆，-1 按 30s 计）*/
    ref_t = 0.0f;
    for (i = 0; i < show_n; i++) {
        float t = (t100[sorted[i]] < 0.0f) ? 30.0f : t100[sorted[i]];
        if (t > ref_t) ref_t = t;
    }
    if (ref_t < 1.0f) ref_t = 1.0f;

    ui_begin_frame();
    settextstyle(26, 0, "宋体"); settextcolor(RGB(255,220,50));
    outtextxy(50, 22, "全库排行榜 — 0-100 km/h 加速时间");
    settextstyle(15, 0, "宋体"); settextcolor(RGB(110,110,135));
    outtextxy(50, 58, "横条越短 = 加速越快");

    for (i = 0; i < show_n; i++) {
        int idx = sorted[i];
        int ry  = bar_y0 + i * (bar_h + bar_gap);
        float t = (t100[idx] < 0.0f) ? 30.0f : t100[idx];
        int fill_w = (int)(t / ref_t * (float)bar_max_w);
        COLORREF col = g_car_colors[i % 3];

        /* 名次 */
        snprintf(buf, sizeof(buf), "#%d", i + 1);
        settextstyle(20, 0, "宋体"); settextcolor(RGB(255,220,50));
        outtextxy(50, ry + 12, buf);

        /* 车名 */
        settextcolor(RGB(210,210,210));
        outtextxy(90, ry + 12, cars[idx].name);

        /* 横向条形 */
        setfillcolor(col);
        solidrectangle(bar_x, ry, bar_x + fill_w, ry + bar_h - 2);

        /* 时间数值 */
        if (t100[idx] < 0.0f) snprintf(buf, sizeof(buf), "> 30 s");
        else                   snprintf(buf, sizeof(buf), "%.2f s", t100[idx]);
        settextstyle(18, 0, "宋体"); settextcolor(col);
        outtextxy(bar_x + fill_w + 8, ry + 12, buf);

        /* 副指标 */
        snprintf(buf, sizeof(buf), "极速 %.0f km/h   制动 %.0f m",
                 vmax[idx] * 3.6f, bdist[idx]);
        settextstyle(13, 0, "宋体"); settextcolor(RGB(130,130,150));
        outtextxy(90, ry + bar_h - 16, buf);
    }

    /* 时间轴 */
    {
        int axis_y = bar_y0 + show_n * (bar_h + bar_gap) + 10;
        setlinecolor(RGB(80,80,100)); setlinestyle(PS_SOLID,1);
        line(bar_x, axis_y, bar_x + bar_max_w, axis_y);
        settextstyle(13, 0, "宋体"); settextcolor(RGB(120,120,140));
        for (j = 0; j <= 5; j++) {
            int tx = bar_x + j * bar_max_w / 5;
            snprintf(buf, sizeof(buf), "%.1fs", ref_t * j / 5.0f);
            outtextxy(tx - 10, axis_y + 6, buf);
            setlinecolor(RGB(50,50,65));
            line(tx, bar_y0, tx, axis_y);
            setlinecolor(RGB(80,80,100));
        }
    }
    wait_any_key(NULL);
}

/* ============================================================ */
/* 发动机外特性曲线                                             */
/* ============================================================ */
void sim_run_ft_curve(Car cars[], int car_count)
{
    int idx;
    Car *car;
    char buf[64];
    float v_kmh, v_ms;
    float Ft_torque, Ft_power, Ft, Fw, Ff, Fr;
    float v_max_kmh = 0.0f, v_knee_kmh = 0.0f;
    float max_F, v_end_kmh;
    int ox, oy, w, h;
    int px1, py1, px2, py2, first;
    float v_step = 0.3f;

    idx = ui_select_car(cars, car_count);
    if (idx < 0) return;
    car = &cars[idx];

    /* 固定量 */
    Ft_torque = physics_drive_force(car->torque_nm, car->gear_ratio, car->wheel_radius_m);
    Ff = physics_rolling_resistance(car->mass_kg);

    /* 找理论极速：Ft(v) <= Fr(v) 的第一个点 */
    for (v_kmh = 1.0f; v_kmh < 400.0f; v_kmh += 0.1f) {
        v_ms = v_kmh / 3.6f;
        Ft_power = (car->power_kw * 1000.0f) / v_ms;
        Ft = (Ft_torque < Ft_power) ? Ft_torque : Ft_power;
        Fw = physics_air_resistance(car->drag_coeff, car->frontal_area_m2, v_ms);
        Fr = Fw + Ff;
        if (Ft <= Fr) { v_max_kmh = v_kmh; break; }
    }
    if (v_max_kmh < 10.0f) v_max_kmh = 200.0f;

    /* 恒扭矩→恒功率拐点速度：v_knee = P*1000 / Ft_T */
    v_knee_kmh = (car->power_kw * 1000.0f) / Ft_torque * 3.6f;

    max_F     = Ft_torque * 1.15f;
    v_end_kmh = v_max_kmh * 1.12f;
    if (v_end_kmh < 60.0f) v_end_kmh = 60.0f;

    /* 图表区域 */
    ox = 90; oy = WIN_HEIGHT - 70; w = WIN_WIDTH - 200; h = WIN_HEIGHT - 170;

    ui_begin_frame();
    settextstyle(24, 0, "宋体"); settextcolor(RGB(255,220,50));
    outtextxy(50, 18, "发动机外特性曲线 — 驱动力 & 行驶阻力");
    settextstyle(18, 0, "宋体"); settextcolor(g_car_colors[0]);
    outtextxy(50, 52, car->name);
    snprintf(buf, sizeof(buf), "%.0f kW   %.0f N.m   %.0f kg",
             car->power_kw, car->torque_nm, car->mass_kg);
    settextstyle(15, 0, "宋体"); settextcolor(RGB(160,160,180));
    outtextxy(50, 76, buf);

    /* ---- 坐标轴 ---- */
    {
        int i;
        setlinecolor(RGB(170,170,170)); setlinestyle(PS_SOLID,1);
        line(ox, oy, ox+w+10, oy);
        line(ox, oy, ox, oy-h-10);
        settextstyle(14, 0, "宋体"); settextcolor(RGB(160,160,160));
        for (i = 1; i <= 5; i++) {
            int tx = ox + i*w/5;
            char tmp[32];
            line(tx, oy, tx, oy+5);
            snprintf(tmp, sizeof(tmp), "%.0f", v_end_kmh*i/5.0f);
            outtextxy(tx-10, oy+8, tmp);
            setlinecolor(RGB(38,38,52)); line(tx, oy-h, tx, oy);
            setlinecolor(RGB(170,170,170));
        }
        for (i = 1; i <= 4; i++) {
            int ty = oy - i*h/4;
            char tmp[32];
            line(ox-5, ty, ox, ty);
            snprintf(tmp, sizeof(tmp), "%.0f", max_F*i/4.0f);
            outtextxy(ox-52, ty-8, tmp);
            setlinecolor(RGB(38,38,52)); line(ox, ty, ox+w, ty);
            setlinecolor(RGB(170,170,170));
        }
        settextcolor(RGB(200,200,200));
        outtextxy(ox+w+12, oy-8, "km/h");
        outtextxy(ox-8, oy-h-24, "F/N");
    }

    /* ---- 驱动力曲线 Ft(v) —— 蓝色 ---- */
    setlinecolor(RGB(80,150,255)); setlinestyle(PS_SOLID,2);
    first = 1;
    for (v_kmh = v_step; v_kmh <= v_end_kmh + v_step; v_kmh += v_step) {
        v_ms = v_kmh / 3.6f;
        Ft_power = (car->power_kw * 1000.0f) / v_ms;
        Ft = (Ft_torque < Ft_power) ? Ft_torque : Ft_power;
        if (Ft < 0.0f) Ft = 0.0f;
        px2 = ox + (int)(v_kmh / v_end_kmh * (float)w);
        py2 = oy - (int)(Ft   / max_F    * (float)h);
        if (py2 < oy-h) py2 = oy-h;
        if (py2 > oy)   py2 = oy;
        if (!first) line(px1, py1, px2, py2);
        px1=px2; py1=py2; first=0;
    }

    /* ---- 行驶阻力曲线 Fr(v) = Fw + Ff —— 红色 ---- */
    setlinecolor(RGB(255,90,90)); setlinestyle(PS_SOLID,2);
    first = 1;
    for (v_kmh = 0.0f; v_kmh <= v_end_kmh + v_step; v_kmh += v_step) {
        v_ms = v_kmh / 3.6f;
        Fw = physics_air_resistance(car->drag_coeff, car->frontal_area_m2, v_ms);
        Fr = Fw + Ff;
        px2 = ox + (int)(v_kmh / v_end_kmh * (float)w);
        py2 = oy - (int)(Fr   / max_F    * (float)h);
        if (py2 < oy-h) py2 = oy-h;
        if (py2 > oy)   py2 = oy;
        if (!first) line(px1, py1, px2, py2);
        px1=px2; py1=py2; first=0;
    }

    /* ---- 100 km/h 参考竖线 ---- */
    if (100.0f <= v_end_kmh) {
        int rx = ox + (int)(100.0f / v_end_kmh * (float)w);
        setlinecolor(RGB(240,230,90)); setlinestyle(PS_DASH,1);
        line(rx, oy-h, rx, oy);
        settextstyle(13, 0, "宋体"); settextcolor(RGB(240,230,90));
        outtextxy(rx+3, oy-h+4, "100km/h");
    }
    /* ---- 极速竖线 ---- */
    if (v_max_kmh > 0.0f && v_max_kmh <= v_end_kmh) {
        int rx = ox + (int)(v_max_kmh / v_end_kmh * (float)w);
        setlinecolor(RGB(100,255,100)); setlinestyle(PS_DASH,1);
        line(rx, oy-h, rx, oy);
        settextstyle(13, 0, "宋体"); settextcolor(RGB(100,255,100));
        snprintf(buf, sizeof(buf), "极速 %.0f", v_max_kmh);
        outtextxy(rx+3, oy-h+20, buf);
    }
    /* ---- 拐点竖线 ---- */
    if (v_knee_kmh > 0.0f && v_knee_kmh < v_end_kmh) {
        int kx = ox + (int)(v_knee_kmh / v_end_kmh * (float)w);
        setlinecolor(RGB(180,180,200)); setlinestyle(PS_DOT,1);
        line(kx, oy-h, kx, oy);
        settextstyle(12, 0, "宋体"); settextcolor(RGB(160,160,190));
        outtextxy(ox+(kx-ox)/2-25, oy-h-22, "恒扭矩");
        if (kx + 60 < ox+w) outtextxy(kx+8, oy-h-22, "恒功率");
    }

    /* ---- 图例 ---- */
    {
        int lx = ox+16, ly = oy-h+10;
        settextstyle(15, 0, "宋体");
        setfillcolor(RGB(80,150,255));  solidrectangle(lx, ly,    lx+30, ly+14);
        settextcolor(RGB(80,150,255));  outtextxy(lx+38, ly,    "驱动力 Ft(v)");
        setfillcolor(RGB(255,90,90));   solidrectangle(lx, ly+24, lx+30, ly+38);
        settextcolor(RGB(255,90,90));   outtextxy(lx+38, ly+24, "行驶阻力 Fr(v)");
    }
    /* ---- 关键参数 ---- */
    {
        int px = WIN_WIDTH - 300;
        settextstyle(16, 0, "宋体");
        settextcolor(RGB(200,200,200));
        snprintf(buf, sizeof(buf), "最大驱动力: %.0f N", Ft_torque);
        outtextxy(px, oy-h+10, buf);
        snprintf(buf, sizeof(buf), "额定功率:   %.0f kW", car->power_kw);
        outtextxy(px, oy-h+32, buf);
        settextcolor(RGB(100,255,100));
        snprintf(buf, sizeof(buf), "理论极速:   %.0f km/h", v_max_kmh);
        outtextxy(px, oy-h+54, buf);
        settextcolor(RGB(240,230,90));
        snprintf(buf, sizeof(buf), "拐点速度:   %.0f km/h", v_knee_kmh);
        outtextxy(px, oy-h+76, buf);
    }
    wait_any_key(NULL);
}
