# 算法设计与分析 - 课外实验作业

**云南大学信息学院 · 2026年春季学期**
**2024级计算机科学与技术专业**

---

## 项目结构

```
.
├── src/                              # 源代码
│   ├── algorithm.c                   # C语言实现（排序 + 0-1背包）
│   ├── plot.py                       # 数据可视化（Matplotlib）
│   └── generate_report.py            # 实验报告生成（python-docx）
├── data/                             # 实验数据
│   ├── sorting_results.csv           # 排序算法比较次数汇总
│   ├── knapsack_results.csv          # 背包算法结果汇总
│   ├── knapsack_detail.csv           # 背包算法详细结果
│   ├── knapsack_1000_items.csv       # 1000个物品统计信息
│   └── subproblems/                  # 子问题规模记录
│       └── *.csv
├── charts/                           # 分析图表
│   ├── sorting_comparison.png        # 排序算法对比（线性+对数）
│   ├── sorting_theoretical_comparison.png  # 实际 vs 理论复杂度
│   ├── subproblem_distribution.png   # 子问题规模分布
│   ├── knapsack_time_by_capacity.png # 背包算法执行时间对比
│   ├── knapsack_greedy_comparison.png # 贪心法不同容量对比
│   └── knapsack_dp_complexity.png    # DP复杂度验证
├── 算法设计与分析实验报告.docx        # 实验报告
└── README.md
```

## 实验内容

### 实验1：排序问题
- **算法**：冒泡排序（蛮力法）、归并排序（分治法）、快速排序（分治法）
- **测试规模**：10 ~ 10,000,000（24个数据点）
- **指标**：比较操作执行次数、子问题规模分布

### 实验2：0-1背包问题
- **算法**：蛮力法、动态规划法、贪心法、回溯法
- **测试规模**：物品数 1,000 ~ 320,000，背包容量 10,000 ~ 1,000,000
- **指标**：执行时间、总价值、算法适用性分析

## 编译与运行

```bash
# 编译
gcc -O2 -o algorithm src/algorithm.c -lm

# 运行实验
./algorithm

# 生成图表
python src/plot.py

# 生成报告
python src/generate_report.py
```

## 实验环境

| 项目 | 配置 |
|------|------|
| OS | Windows 11 Pro |
| CPU | Intel(R) Core(TM) i7-14700HX @ 2.10 GHz |
| RAM | 16 GB |
| 编译器 | GCC 14.2.0 (-O2) |
| Python | 3.14 + Matplotlib + python-docx |
