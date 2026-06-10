#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
算法设计与分析 - 实验数据可视化
==============================
生成排序算法比较次数对比图 和 0-1背包算法执行时间对比图
"""

import csv
import os
import matplotlib
matplotlib.use('Agg')  # 非交互式后端
import matplotlib.pyplot as plt
import numpy as np

# 路径解析：无论从哪里运行，均定位到项目根目录
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
DATA = os.path.join(ROOT, 'data')
CHART = os.path.join(ROOT, 'charts')
SUB = os.path.join(DATA, 'subproblems')
os.makedirs(CHART, exist_ok=True)

# ================================================================
# 全局样式设置
# ================================================================
plt.rcParams['font.size'] = 12
plt.rcParams['axes.unicode_minus'] = False
plt.rcParams['mathtext.default'] = 'regular'
plt.rcParams['font.family'] = 'sans-serif'

# 尝试使用中文字体
for font in ['SimHei', 'Microsoft YaHei', 'WenQuanYi Micro Hei', 'Noto Sans CJK SC', 'DejaVu Sans']:
    try:
        plt.rcParams['font.sans-serif'] = [font]
        # 测试字体是否可用
        fig = plt.figure()
        fig.text(0.5, 0.5, '测试', fontsize=12)
        plt.close(fig)
        break
    except:
        continue

# ================================================================
# 图1: 排序算法比较次数对比
# ================================================================
def plot_sorting_comparison():
    """绘制排序算法比较次数 vs 输入规模的对比曲线"""
    data = {'Bubble': {'n': [], 'comp': []},
            'Merge': {'n': [], 'comp': []},
            'Quick': {'n': [], 'comp': []}}

    with open(os.path.join(DATA, 'sorting_results.csv'), 'r') as f:
        reader = csv.DictReader(f)
        for row in reader:
            algo = row['Algorithm']
            n = int(row['Size'])
            comp = int(row['Comparisons'])
            # 过滤掉冒泡排序被跳过的数据点（comp=0表示跳过）
            if comp == 0 and algo == 'Bubble':
                continue
            data[algo]['n'].append(n)
            data[algo]['comp'].append(comp)

    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(14, 6))

    # 子图1: 线性坐标（冒泡的O(n²)太大，仅显示小规模）
    for algo, style, label in [('Bubble', 'ro-', 'Bubble O(n^2)'),
                                ('Merge', 'bs-', 'Merge O(n log n)'),
                                ('Quick', 'g^-', 'Quick O(n log n)')]:
        d = data[algo]
        ax1.plot(d['n'], d['comp'], style, label=label, markersize=5, linewidth=1.5)

    ax1.set_xlabel('Input Size (n)')
    ax1.set_ylabel('Comparison Count')
    ax1.set_title('Sorting Algorithm Comparison (Linear Scale)')
    ax1.legend(fontsize=9)
    ax1.grid(True, alpha=0.3)

    # 子图2: 双对数坐标（全范围，清晰展示O(n²) vs O(n log n) 斜率差异）
    for algo, style, label in [('Bubble', 'ro-', 'Bubble O(n^2)'),
                                ('Merge', 'bs-', 'Merge O(n log n)'),
                                ('Quick', 'g^-', 'Quick O(n log n)')]:
        d = data[algo]
        ax2.loglog(d['n'], d['comp'], style, label=label, markersize=5, linewidth=1.5)

    ax2.set_xlabel('Input Size (n)')
    ax2.set_ylabel('Comparison Count')
    ax2.set_title('Sorting Algorithm Comparison (Log-Log Scale)')
    ax2.legend(fontsize=9)
    ax2.grid(True, alpha=0.3)

    # 理论复杂度参考线（扩展至1000万范围）
    n_ref = np.logspace(1, 7, 200)
    ax2.loglog(n_ref, n_ref**2 / 2, 'r--', alpha=0.35, linewidth=1.2, label='Theory: n^2/2')
    ax2.loglog(n_ref, n_ref * np.log2(n_ref), 'b--', alpha=0.35, linewidth=1.2, label='Theory: n log_2 n')
    ax2.legend(fontsize=9)

    plt.tight_layout()
    plt.savefig(os.path.join(CHART, 'sorting_comparison.png'), dpi=150, bbox_inches='tight')
    plt.close()
    print("图1已保存: sorting_comparison.png")


# ================================================================
# 图2: 0-1背包算法执行时间对比 (按背包容量分组)
# ================================================================
def plot_knapsack_time():
    """绘制背包算法执行时间 vs 物品数量的对比曲线"""

    # 读取数据
    data = {}  # data[algo][C] = [(n, time), ...]
    with open(os.path.join(DATA, 'knapsack_results.csv'), 'r') as f:
        reader = csv.DictReader(f)
        for row in reader:
            algo = row['Algorithm']
            n = int(row['n_items'])
            C = int(row['Capacity'])
            time_ms = float(row['Time_ms'])
            feasible = int(row['Feasible'])

            if feasible == 0 and time_ms == 0:
                continue  # 跳过的测试

            if algo not in data:
                data[algo] = {}
            if C not in data[algo]:
                data[algo][C] = []
            data[algo][C].append((n, time_ms))

    capacities = [10000, 100000, 1000000]
    algo_styles = {
        'BruteForce': ('ro-', 'Brute Force'),
        'DP': ('bs-', 'Dynamic Programming'),
        'Greedy': ('g^-', 'Greedy'),
        'Backtrack': ('mD-', 'Backtracking'),
    }

    # 为每个容量创建子图
    fig, axes = plt.subplots(1, 3, figsize=(18, 6))

    for idx, C in enumerate(capacities):
        ax = axes[idx]
        for algo_key, (style, label) in algo_styles.items():
            if algo_key in data and C in data[algo_key]:
                points = sorted(data[algo_key][C])
                if points:
                    ns, times = zip(*points)
                    ax.plot(ns, times, style, label=label, markersize=5)

        ax.set_xlabel('Number of Items (n)')
        ax.set_ylabel('Execution Time (ms)')
        ax.set_title(f'0-1 Knapsack (C={C:,})')
        ax.legend(fontsize=8)
        ax.grid(True, alpha=0.3)
        ax.set_xscale('log')
        ax.set_yscale('log')

    plt.tight_layout()
    plt.savefig(os.path.join(CHART, 'knapsack_time_by_capacity.png'), dpi=150, bbox_inches='tight')
    plt.close()
    print("图2已保存: knapsack_time_by_capacity.png")

    # 额外: 贪心法在不同容量下的对比 (贪心法总是能跑完所有规模)
    fig2, ax = plt.subplots(1, 1, figsize=(10, 6))

    colors = ['#2196F3', '#FF9800', '#4CAF50']
    if 'Greedy' in data:
        for idx, C in enumerate(capacities):
            if C in data['Greedy']:
                points = sorted(data['Greedy'][C])
                if points:
                    ns, times = zip(*points)
                    ax.plot(ns, times, 'o-', color=colors[idx],
                            label=f'C={C:,}', markersize=6)

    ax.set_xlabel('Number of Items (n)')
    ax.set_ylabel('Execution Time (ms)')
    ax.set_title('Greedy Algorithm: Time vs Items for Different Capacities')
    ax.legend()
    ax.grid(True, alpha=0.3)
    # 理论 O(n log n) 参考线
    n_ref = np.array([min(ns), max(ns)])
    scale = np.median([t / (n * np.log2(n)) for n, t in zip(ns, times) if n > 0])
    ax.plot(n_ref, scale * n_ref * np.log2(n_ref), 'k--', alpha=0.5,
            label='O(n log n) reference')
    ax.legend()

    plt.tight_layout()
    plt.savefig(os.path.join(CHART, 'knapsack_greedy_comparison.png'), dpi=150, bbox_inches='tight')
    plt.close()
    print("图3已保存: knapsack_greedy_comparison.png")


# ================================================================
# 图3: DP算法理论vs实际时间复杂度
# ================================================================
def plot_dp_complexity():
    """对比DP算法实际运行时间与理论O(n*C)复杂度"""
    data = []
    with open(os.path.join(DATA, 'knapsack_results.csv'), 'r') as f:
        reader = csv.DictReader(f)
        for row in reader:
            if row['Algorithm'] == 'DP' and float(row['Time_ms']) > 0:
                n = int(row['n_items'])
                C = int(row['Capacity'])
                time_ms = float(row['Time_ms'])
                data.append((n, C, time_ms))

    if not data:
        print("没有DP算法数据，跳过DP复杂度分析图")
        return

    fig, ax = plt.subplots(1, 1, figsize=(10, 6))

    nC_values = [n * C for n, C, _ in data]
    times = [t for _, _, t in data]

    ax.scatter(nC_values, times, c='blue', alpha=0.7, s=40)
    ax.set_xlabel('n × C (Problem Size)')
    ax.set_ylabel('Execution Time (ms)')
    ax.set_title('DP Algorithm: Actual Time vs Theoretical O(n×C)')
    ax.grid(True, alpha=0.3)

    # 线性拟合
    if len(nC_values) > 1:
        coeffs = np.polyfit(nC_values, times, 1)
        x_fit = np.linspace(min(nC_values), max(nC_values), 100)
        y_fit = np.polyval(coeffs, x_fit)
        ax.plot(x_fit, y_fit, 'r--', label=f'Linear fit (slope={coeffs[0]:.2e})')
        ax.legend()

    plt.tight_layout()
    plt.savefig(os.path.join(CHART, 'knapsack_dp_complexity.png'), dpi=150, bbox_inches='tight')
    plt.close()
    print("图4已保存: knapsack_dp_complexity.png")


# ================================================================
# 图4: 排序算法子问题规模分布 (以n=1000为例)
# ================================================================
def plot_subproblem_distribution():
    """绘制归并排序和快速排序的子问题规模分布——改进版（更少柱、更大间距）"""
    fig, axes = plt.subplots(1, 2, figsize=(14, 5))

    for idx, algo in enumerate(['merge', 'quick']):
        ax = axes[idx]
        filename = os.path.join(SUB, f'{algo}_subproblems_n1000.csv')

        if not os.path.exists(filename):
            ax.set_title(f'{algo.capitalize()} Sort - No Data')
            continue

        sizes = []
        with open(filename, 'r') as f:
            reader = csv.DictReader(f)
            for row in reader:
                sizes.append(int(row['SubproblemSize']))

        # 使用较少的bins + 柱子间距 (rwidth)，避免挤在一起
        # 合并排序子问题规模呈2的幂次分布，使用对数间隔的bins
        bins = np.logspace(np.log10(1), np.log10(max(sizes) + 1), 25)
        ax.hist(sizes, bins=bins, alpha=0.75, color='steelblue',
                edgecolor='white', rwidth=0.85, linewidth=0.5)
        ax.set_xscale('log')
        ax.set_xlabel('Subproblem Size (log scale)')
        ax.set_ylabel('Frequency')
        ax.set_title(f'{algo.capitalize()} Sort Subproblem Sizes (n=1000)')
        ax.axvline(x=np.mean(sizes), color='red', linestyle='--',
                   linewidth=1.5, label=f'Mean={np.mean(sizes):.1f}')
        ax.legend()
        ax.grid(True, alpha=0.2, axis='y')

    plt.tight_layout()
    plt.savefig(os.path.join(CHART, 'subproblem_distribution.png'), dpi=150, bbox_inches='tight')
    plt.close()
    print("图5已保存: subproblem_distribution.png")


# ================================================================
# 综合报告图: 排序算法 + 理论复杂度对比表
# ================================================================
def plot_theoretical_comparison():
    """理论复杂度与实际测量值对比"""
    data = {'Bubble': {'n': [], 'comp': []},
            'Merge': {'n': [], 'comp': []},
            'Quick': {'n': [], 'comp': []}}

    with open(os.path.join(DATA, 'sorting_results.csv'), 'r') as f:
        reader = csv.DictReader(f)
        for row in reader:
            algo = row['Algorithm']
            n = int(row['Size'])
            comp = int(row['Comparisons'])
            if comp == 0 and algo == 'Bubble':
                continue
            data[algo]['n'].append(n)
            data[algo]['comp'].append(comp)

    fig, ax = plt.subplots(1, 1, figsize=(12, 7))

    # 绘制实际数据
    for algo, style, label in [('Bubble', 'ro-', 'Bubble (actual)'),
                                ('Merge', 'bs-', 'Merge (actual)'),
                                ('Quick', 'g^-', 'Quick (actual)')]:
        d = data[algo]
        ax.loglog(d['n'], d['comp'], style, label=label, markersize=6, linewidth=1.5, alpha=0.85)

    # 理论曲线（扩展至n=10^7）
    n_range = np.logspace(1, 7, 300)
    # Bubble: O(n^2), 比较次数 ~ n(n-1)/2
    ax.loglog(n_range, n_range * (n_range - 1) / 2, 'r--', alpha=0.4,
              linewidth=1.5, label='Theory: n(n-1)/2')
    # Merge/Quick: O(n log n)
    ax.loglog(n_range, n_range * np.log2(n_range), 'b--', alpha=0.4,
              linewidth=1.5, label='Theory: n log_2 n')

    ax.set_xlabel('Input Size (n)', fontsize=13)
    ax.set_ylabel('Comparison Count', fontsize=13)
    ax.set_title('Sorting Algorithms: Actual vs Theoretical Complexity (n up to 10^7)', fontsize=14)
    ax.legend(fontsize=9, loc='upper left')
    ax.grid(True, alpha=0.3)

    plt.tight_layout()
    plt.savefig(os.path.join(CHART, 'sorting_theoretical_comparison.png'), dpi=150, bbox_inches='tight')
    plt.close()
    print("图6已保存: sorting_theoretical_comparison.png")


# ================================================================
# 主函数
# ================================================================
if __name__ == '__main__':
    print("=" * 60)
    print("算法设计与分析 - 实验数据可视化")
    print("=" * 60)

    plot_sorting_comparison()
    plot_knapsack_time()
    plot_dp_complexity()
    plot_subproblem_distribution()
    plot_theoretical_comparison()

    print("\n" + "=" * 60)
    print("所有图表已生成完成!")
    print("生成的文件（保存在 charts/ 目录）:")
    for f in sorted(os.listdir(CHART)):
        print(f"  - charts/{f}")
    print("=" * 60)
