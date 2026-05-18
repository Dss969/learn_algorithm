# 车辆性能仿真系统

汽车工程专业 C 语言期末课设。基于《汽车理论》第 5 版公式，使用 EasyX 图形库实现可视化仿真。

---

## 功能模块

| 键 | 功能 | 说明 |
|-----|------|------|
| `1` | 加速测试 | 0→100 km/h 仪表盘动画，车身在滚动路面上前进，结束显示 v-t 曲线 |
| `2` | 制动测试 | 100 km/h 紧急制动，空格键触发，显示制动距离 + s-t 曲线 |
| `3` | 车型对比 | 选 2–3 车，共享 Y 轴叠加 v-t 曲线 + 参数表格 |
| `4` | 车型管理 | 键盘录入 / 删除车型，实时写入 `data/cars.dat` |
| `5` | 全库排行榜 | 批量仿真所有车型，按 0-100 时间排序，横向条形图 |
| `6` | 外特性曲线 | 驱动力 Ft(v) vs 行驶阻力 Fr(v)，标注拐点、极速、区域 |
| `0` / `Esc` | 退出 | |

---

## 环境要求

- Windows 10 / 11
- **VS Code** + **MinGW-w64**（g++ 版本 ≥ 9）
- **EasyX 图形库**（MinGW 专用版）

> EasyX 内部使用了 C++ 特性，所以即便源文件是纯 C 写法，也必须用 **g++ 以 C++ 模式**编译。

---

## 配置步骤（VS Code + MinGW）

### 第 1 步 安装 MinGW-w64

推荐通过 **MSYS2** 安装：

1. 下载安装 MSYS2
2. 打开 MSYS2 终端，执行：
   ```bash
   pacman -S mingw-w64-x86_64-gcc
   ```
3. 将 `C:\msys64\mingw64\bin` 添加到系统 **PATH** 环境变量
4. 打开新的 CMD 窗口，验证：
   ```
   g++ --version
   ```
   能打印版本号就表示成功。

### 第 2 步 安装 EasyX（MinGW 专用版）

1. 访问 [https://easyx.cn](https://easyx.cn)，在下载页选择下载 **适用于 MinGW 的版本**（压缩包）
2. 解压后将文件手动复制到 MinGW 目录：

   | EasyX 文件 | 复制到 |
   |------------|--------|
   | `graphics.h` | `C:\msys64\mingw64\include\` |
   | `easyx.h` | `C:\msys64\mingw64\include\` |
   | `libeasyx.a` | `C:\msys64\mingw64\lib\` |

3. 验证：在 src 目录下手动运行编译命令，不报错即成功。

### 第 3 步 配置 VS Code 编译任务

在仓库根目录创建 `.vscode/tasks.json`：

```json
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "build vehicle-sim",
            "type": "shell",
            "command": "g++",
            "args": [
                "-x", "c++",
                "main.c", "car.c", "physics.c", "ui.c", "simulation.c",
                "-o", "../vehicle_sim.exe",
                "-leasyx", "-lgdi32", "-lgdiplus", "-lole32", "-lwinmm",
                "-fexec-charset=GBK",
                "-Wall"
            ],
            "options": {
                "cwd": "${workspaceFolder}/vehicle-performance-sim/src"
            },
            "group": { "kind": "build", "isDefault": true },
            "problemMatcher": ["$gcc"]
        }
    ]
}
```

**编译选项说明**

| 选项 | 作用 |
|------|------|
| `-x c++` | 强制以 C++ 模式编译（EasyX 必须） |
| `-fexec-charset=GBK` | 将源文件（UTF-8）中的中文字符串实际转为 GBK，否则 EasyX `outtextxy` 显示乱码 |
| `-leasyx -lgdi32 ...` | 链接 EasyX 及 Windows 图形库 |

### 第 4 步 创建输出目录

```bash
mkdir vehicle-performance-sim/data
```

`data/` 目录必须存在，程序首次运行时将在此生成 `cars.dat`。

---

## 编译与运行

```bash
# VS Code 中按 Ctrl+Shift+B 触发编译任务，或手动执行：
cd vehicle-performance-sim/src
g++ -x c++ main.c car.c physics.c ui.c simulation.c \
    -o ../vehicle_sim.exe \
    -leasyx -lgdi32 -lgdiplus -lole32 -lwinmm \
    -fexec-charset=GBK

# 运行
..\vehicle_sim.exe
```

> **连接错误?** 确认 `libeasyx.a` 已复制到 MinGW `lib/` 目录，且编译命令用的是
> `g++` 而非 `gcc`。

---

## 操作说明

### 选车界面

- `↑` / `↓` 键移动高亮
- `Enter` 确认选择
- `Esc` 返回上一级菜单

### 成功运行后的界面

- **加速测试**：动画自动播放，任意键进入 v-t 曲线
- **制动测试**：需明确按**空格键**触发制动
- **车型对比**：选完第 2 辆后可按 Esc 直接开始
- **全库排行榜**：自动一次计算全部车型，稍候等待
- **外特性曲线**：选车后自动绘制，无需其它操作

---

## 物理公式

公式来源：《汽车理论》第 5 版（余志生著）

| 公式 | 含义 |
|------|------|
| `Ft = min(T·i/r, P/v)` | 驱动力(N)；低速取扭矩限，高速取功率限 |
| `Fw = ½·ρ·Cd·A·v²` | 空气阻力(N)，ρ=1.225 kg/m³ |
| `Ff = f·m·g` | 滚动阻力(N)，f=0.015 |
| `a = (Ft−Fw−Ff) / m` | 净加速度(m/s²) |
| `v(t+dt) = v(t) + a·dt` | 步进积分，dt=0.02 s |
| `s = v² / (2·a_brake)` | 制动距离(m) |

---

## 代码结构

```
vehicle-performance-sim/
├── src/
│   ├── main.c          入口、主循环、模块调度
│   ├── car.h / car.c   Car 结构体定义，二进制文件读写
│   ├── physics.h/.c    汽车理论公式实现（无图形依赖）
│   ├── ui.h / ui.c     EasyX 绘制层（仪表盘、跑道、曲线、面板）
│   └── simulation.h/.c 加速/制动/对比/排行榜/外特性曲线完整流程
├── data/
│   └── cars.dat        车型二进制库（首次运行自动创建）
└── vehicle_sim.exe 编译输出（.gitignore 过滤）
```

**分层设计**：`physics.c` 纯算法，`ui.c` 纯绘图，`simulation.c` 调用两者组合成完整展示流程。

---

## 常见问题

| 现象 | 解决方法 |
|------|----------|
| 编译报错 `graphics.h not found` | 确认 EasyX `.h` 文件已复制到 MinGW `include/` |
| 连接报错 `libeasyx not found` | 确认 `libeasyx.a` 已复制到 MinGW `lib/` |
| 窗口弹出立刻关闭 | 可能 `data/` 目录不存在，手动创建 |
| 中文显示乱码 | 确认编译命令包含 `-fexec-charset=GBK` |
| 空格键不响应 | 需要先选车，制动测试页面展示后再按 |
| 菜单键入无反应 | 确认不是 `gcc` 编译，必须用 `g++ -x c++` |

---

## 预设车型参数来源

| 车型 | 功率 kW | 扭矩 N·m | 质量 kg | 0-100 理论 |
|------|---------|----------|--------|--------|
| 比亚迪汉 EV | 380 | 700 | 2050 | ~3.3 s |
| Tesla Model 3 长续航 | 358 | 659 | 1830 | ~3.4 s |
| 本田思域 1.5T | 134 | 240 | 1330 | ~7.5 s |
| 大众高尔夫 GTI | 180 | 370 | 1430 | ~6.1 s |
| 五菱宏光 MINI EV | 20 | 85 | 700 | 未破百 |

数据来自公开资料，作教学演示用途。
