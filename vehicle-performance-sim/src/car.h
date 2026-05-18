/*
 * car.h  —  Car 结构体定义 & 车型库接口
 */
#ifndef CAR_H
#define CAR_H

/* 车型库容量上限 */
#define MAX_CARS  50
/* 二进制数据文件路径（相对于可执行文件所在目录） */
#define CAR_FILE  "data/cars.dat"

typedef struct {
    char  name[32];         /* 车型名称 */
    float power_kw;         /* 额定功率 kW */
    float torque_nm;        /* 最大扭矩 N·m */
    float mass_kg;          /* 整备质量 kg */
    float drag_coeff;       /* 风阻系数 Cd */
    float frontal_area_m2;  /* 迎风面积 m² */
    float gear_ratio;       /* 综合传动比（变速箱最低档 × 主减速比） */
    float wheel_radius_m;   /* 轮胎滚动半径 m */
    float max_brake_decel;  /* 最大制动减速度 m/s² */
} Car;

/* 从 CAR_FILE 读取所有车型；返回 0=成功，-1=文件不存在 */
int  car_load(Car cars[], int *count);

/* 将当前数组全量写回 CAR_FILE；返回 0=成功，-1=写入失败 */
int  car_save(Car cars[], int count);

/* 追加一辆车到数组末尾；返回 0=成功，-1=已满 */
int  car_add(Car cars[], int *count, Car c);

/* 按索引删除（末尾元素覆盖空位）；返回 0=成功，-1=越界 */
int  car_delete(Car cars[], int *count, int index);

/* 控制台打印所有车型（调试辅助） */
void car_print_list(Car cars[], int count);

/* 写入 5 辆预设车型（首次运行、cars.dat 不存在时调用） */
void car_init_presets(Car cars[], int *count);

#endif /* CAR_H */
