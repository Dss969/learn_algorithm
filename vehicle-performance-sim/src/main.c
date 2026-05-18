/*
 * main.c  —  车辆性能仿真入口
 *
 * 职责：
 *   1. 加载（或初始化）车型库
 *   2. 创建 EasyX 图形窗口
 *   3. 主循环：显示菜单 → 分发功能模块 → 持久化
 *   4. 干净退出
 */
#include <stdio.h>
#include "car.h"
#include "ui.h"
#include "simulation.h"

int main(void)
{
    Car  cars[MAX_CARS];   /* 车型数组（栈分配，结构体数组，无链表） */
    int  car_count = 0;
    int  choice;
    int  running = 1;

    /* ① 加载车型库；若 cars.dat 不存在则写入 5 辆预设车型 */
    if (car_load(cars, &car_count) != 0) {
        car_init_presets(cars, &car_count);
        car_save(cars, car_count);
    }

    /* ② 创建 EasyX 图形窗口（1280×720），启动即有画面 */
    ui_init();

    /* ③ 主循环：每次循环展示主菜单，按选择分发给对应模块 */
    while (running) {
        choice = ui_main_menu();   /* 阻塞直到用户按键选择 */

        switch (choice) {
            case 1:
                /* 加速测试：选车 → 仿真 → 仪表盘动画 → v-t 曲线 */
                sim_run_accel(cars, car_count);
                break;

            case 2:
                /* 制动测试：选车 → 100 km/h → 空格触发 → s-t 曲线 */
                sim_run_brake(cars, car_count);
                break;

            case 3:
                /* 车型对比：选 2-3 辆 → 叠加曲线 + 表格 */
                sim_run_comparison(cars, car_count);
                break;

            case 4:
                /* 车型管理：键盘录入 / 删除，完成后立即持久化 */
                sim_run_car_manager(cars, &car_count);
                car_save(cars, car_count);
                break;

            case 0:
            default:
                /* 退出 */
                running = 0;
                break;
        }
    }

    /* ④ 关闭 EasyX 窗口，程序干净退出 */
    ui_close();
    return 0;
}
