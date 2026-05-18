/*
 * main.c  —  车辆性能仿真入口
 */
#include <stdio.h>
#include "car.h"
#include "ui.h"
#include "simulation.h"

int main(void)
{
    Car  cars[MAX_CARS];
    int  car_count = 0;
    int  choice, running = 1;

    if (car_load(cars, &car_count) != 0) {
        car_init_presets(cars, &car_count);
        car_save(cars, car_count);
    }

    ui_init();

    while (running) {
        choice = ui_main_menu();
        switch (choice) {
            case 1: sim_run_accel(cars, car_count);                   break;
            case 2: sim_run_brake(cars, car_count);                   break;
            case 3: sim_run_comparison(cars, car_count);              break;
            case 4: sim_run_car_manager(cars, &car_count);
                    car_save(cars, car_count);                        break;
            case 5: sim_run_leaderboard(cars, car_count);             break;
            case 6: sim_run_ft_curve(cars, car_count);                break;
            case 0:
            default: running = 0; break;
        }
    }

    ui_close();
    return 0;
}
