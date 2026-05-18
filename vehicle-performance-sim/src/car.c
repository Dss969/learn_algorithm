/*
 * car.c  —  车型库读写实现
 *
 * 存储格式：纯二进制，逐条 fwrite(Car)，无头部元数据。
 * 读取时按 sizeof(Car) 逐条 fread，直到 EOF 或达到 MAX_CARS。
 */
#include <stdio.h>
#include <string.h>
#include "car.h"

/* ------------------------------------------------------------------ */
/* 从文件读取所有车型                                                    */
/* ------------------------------------------------------------------ */
int car_load(Car cars[], int *count)
{
    FILE *fp;

    *count = 0;
    fp = fopen(CAR_FILE, "rb");
    if (fp == NULL) return -1;   /* 文件不存在，由调用方写入预设 */

    while (*count < MAX_CARS &&
           fread(&cars[*count], sizeof(Car), 1, fp) == 1) {
        (*count)++;
    }
    fclose(fp);
    return 0;
}

/* ------------------------------------------------------------------ */
/* 全量写回文件（每次保存覆盖整个文件）                                   */
/* ------------------------------------------------------------------ */
int car_save(Car cars[], int count)
{
    FILE *fp;

    fp = fopen(CAR_FILE, "wb");
    if (fp == NULL) return -1;

    fwrite(cars, sizeof(Car), count, fp);
    fclose(fp);
    return 0;
}

/* ------------------------------------------------------------------ */
/* 追加一辆车                                                           */
/* ------------------------------------------------------------------ */
int car_add(Car cars[], int *count, Car c)
{
    if (*count >= MAX_CARS) return -1;
    cars[*count] = c;
    (*count)++;
    return 0;
}

/* ------------------------------------------------------------------ */
/* 按索引删除：用末尾元素填入空位，避免移动整个数组                         */
/* ------------------------------------------------------------------ */
int car_delete(Car cars[], int *count, int index)
{
    if (index < 0 || index >= *count) return -1;
    cars[index] = cars[*count - 1];
    (*count)--;
    return 0;
}

/* ------------------------------------------------------------------ */
/* 控制台调试输出                                                        */
/* ------------------------------------------------------------------ */
void car_print_list(Car cars[], int count)
{
    int i;
    printf("共 %d 辆车型：\n", count);
    for (i = 0; i < count; i++) {
        printf("[%d] %-24s  %.0f kW  %.0f N·m  %.0f kg\n",
               i, cars[i].name,
               cars[i].power_kw,
               cars[i].torque_nm,
               cars[i].mass_kg);
    }
}

/* ------------------------------------------------------------------ */
/* 写入 5 辆预设车型                                                     */
/* 参数来源：公开资料，用于教学演示，不保证完全精准                          */
/* ------------------------------------------------------------------ */
void car_init_presets(Car cars[], int *count)
{
    Car c;
    *count = 0;

    /* ① 比亚迪汉 EV（2023款，双电机四驱旗舰） */
    memset(&c, 0, sizeof(c));
    strcpy(c.name,          "比亚迪汉EV");
    c.power_kw            = 380.0f;   /* 前后双电机合计峰值功率 */
    c.torque_nm           = 700.0f;
    c.mass_kg             = 2050.0f;
    c.drag_coeff          = 0.233f;
    c.frontal_area_m2     = 2.37f;
    c.gear_ratio          = 9.0f;     /* 电机直驱，等效单速传动比 */
    c.wheel_radius_m      = 0.338f;   /* 245/45 R19 */
    c.max_brake_decel     = 9.0f;
    car_add(cars, count, c);

    /* ② Tesla Model 3 长续航（2023款，双电机） */
    memset(&c, 0, sizeof(c));
    strcpy(c.name,          "Tesla Model3长续航");
    c.power_kw            = 358.0f;
    c.torque_nm           = 659.0f;
    c.mass_kg             = 1830.0f;
    c.drag_coeff          = 0.23f;
    c.frontal_area_m2     = 2.22f;
    c.gear_ratio          = 9.0f;
    c.wheel_radius_m      = 0.332f;   /* 235/45 R18 */
    c.max_brake_decel     = 9.5f;
    car_add(cars, count, c);

    /* ③ 本田思域 1.5T（2022款，CVT最大传动比×主减速比≈12.5） */
    memset(&c, 0, sizeof(c));
    strcpy(c.name,          "本田思域1.5T");
    c.power_kw            = 134.0f;
    c.torque_nm           = 240.0f;
    c.mass_kg             = 1330.0f;
    c.drag_coeff          = 0.28f;
    c.frontal_area_m2     = 2.18f;
    c.gear_ratio          = 12.5f;
    c.wheel_radius_m      = 0.318f;   /* 235/40 R18 */
    c.max_brake_decel     = 9.0f;
    car_add(cars, count, c);

    /* ④ 大众高尔夫 GTI（Mk8，2023款，7速DSG） */
    memset(&c, 0, sizeof(c));
    strcpy(c.name,          "大众高尔夫GTI");
    c.power_kw            = 180.0f;
    c.torque_nm           = 370.0f;
    c.mass_kg             = 1430.0f;
    c.drag_coeff          = 0.30f;
    c.frontal_area_m2     = 2.19f;
    c.gear_ratio          = 11.8f;
    c.wheel_radius_m      = 0.320f;   /* 225/40 R18 */
    c.max_brake_decel     = 9.8f;
    car_add(cars, count, c);

    /* ⑤ 五菱宏光 MINI EV（2022款，单电机前驱） */
    memset(&c, 0, sizeof(c));
    strcpy(c.name,          "五菱宏光MINI EV");
    c.power_kw            = 20.0f;
    c.torque_nm           = 85.0f;
    c.mass_kg             = 700.0f;
    c.drag_coeff          = 0.35f;
    c.frontal_area_m2     = 2.05f;
    c.gear_ratio          = 8.0f;
    c.wheel_radius_m      = 0.267f;   /* 145/70 R12 */
    c.max_brake_decel     = 7.5f;
    car_add(cars, count, c);
}
