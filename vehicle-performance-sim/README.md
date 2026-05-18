# 车辆性能仿真 Vehicle Performance Simulator

汽车工程专业 C 语言期末课设 —— 结合汽车理论公式，使用 EasyX 图形库实现可视化仿真。

## 功能概览

| 模块 | 说明 |
|------|------|
| 加速测试 | 0-100 km/h 动态仿真，仪表盘动画 + v-t 曲线 |
| 制动测试 | 从 100 km/h 紧急制动，显示制动距离 + s-t 曲线 |
| 车型对比 | 最多 3 辆车叠加曲线，彩色图例 + 参数表格 |
| 车型管理 | 键盘录入 / 删除，持久化到 data/cars.dat |

## 环境要求

- Windows 10 / 11
- Visual Studio 2019+（社区版）或 MinGW-w64 + VS Code
- **EasyX 图形库**（必须安装，见下方步骤）

## EasyX 安装步骤

1. 访问 https://easyx.cn 下载最新安装包（EasyX for VC）
2. 运行安装程序，勾选你的 Visual Studio 版本
3. 安装完成后 `graphics.h` 与 `easyx.lib` 会自动写入 VS 的 include / lib 目录

> **MinGW 用户**：在 EasyX 官网下载 MinGW 版压缩包，将 `.h` 手动复制到 MinGW 的 `include/` 目录，`.a` 复制到 `lib/` 目录。

## 编译方式

### Visual Studio（推荐）

1. 新建「空白 C++ 项目」（EasyX 需要 C++ 链接器，但源码本身是纯 C 写法）
2. 将 `src/` 目录下全部 `.c` 文件加入项目
3. 项目属性 → 高级 → 字符集 → **使用多字节字符集**（中文显示必须）
4. 直接 F5 运行即可

### VS Code + MinGW

```bash
cd vehicle-performance-sim/src
gcc main.c car.c physics.c ui.c simulation.c \
    -o ../vehicle_sim.exe \
    -leasyx -lgdi32 -lgdiplus -lole32 -lwinmm
```

## 项目结构

```
vehicle-performance-sim/
├── src/
│   ├── main.c           # 入口、主菜单调度、主循环
│   ├── car.h / car.c    # Car 结构体、车型库读写
│   ├── physics.h / .c   # 汽车理论物理公式
│   ├── ui.h / ui.c      # EasyX 绘制（仪表盘、曲线、面板）
│   └── simulation.h/.c  # 加速 / 制动 / 对比仿真主流程
├── data/
│   └── cars.dat         # 车型二进制数据（首次运行自动生成）
├── assets/              # 留空，后期放车辆位图
└── README.md
```

## 物理公式

公式来源：《汽车理论》第 5 版（余志生著）

| 公式 | 含义 |
|------|------|
| `Ft = T·i / r` | 驱动力（N） |
| `Fw = ½·ρ·Cd·A·v²` | 空气阻力（N），ρ = 1.225 kg/m³ |
| `Ff = f·m·g` | 滚动阻力（N），f = 0.015 |
| `a = (Ft − Fw − Ff) / m` | 净加速度（m/s²） |
| `v(t+dt) = v(t) + a·dt` | 速度积分，dt = 0.02 s |
| `s = v² / (2·a_brake)` | 制动距离（m） |

## 预设车型

| 车型 | 功率 kW | 扭矩 N·m | 质量 kg |
|------|---------|----------|--------|
| 比亚迪汉 EV | 380 | 700 | 2050 |
| Tesla Model 3 长续航 | 358 | 659 | 1830 |
| 本田思域 1.5T | 134 | 240 | 1330 |
| 大众高尔夫 GTI | 180 | 370 | 1430 |
| 五菱宏光 MINI EV | 20 | 85 | 700 |

## 演示流程建议

1. 启动程序，主菜单自动弹出（无黑屏等待）
2. 选「加速测试」→ 选「比亚迪汉 EV」→ 观察仪表盘动画和 v-t 曲线
3. 选「车型对比」→ 选 3 辆车 → 对比叠加曲线 + 参数表格
4. 选「制动测试」→ 等仪表盘到 100 → 按空格触发制动
5. 选「车型管理」→ 演示键盘录入自定义车型

## Git 提交阶段

| 提交 | 内容 |
|------|------|
| feat: 项目骨架 | 文件结构、Car 数据模型、main 主循环、预设车型写入 |
| feat: 物理引擎 | 驱动力 / 阻力 / 加速度 / 制动距离全部公式实现 |
| feat: EasyX UI | 仪表盘、跑道、曲线、参数面板绘制 |
| feat: 仿真联调 | 加速 / 制动 / 对比完整动画流程 |
