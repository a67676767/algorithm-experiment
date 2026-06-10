/**
 * 算法设计与分析 - 课外实验作业
 * ================================
 * 实验1: 排序问题 (冒泡排序、归并排序、快速排序)
 * 实验2: 0-1背包问题 (蛮力法、动态规划法、贪心法、回溯法)
 *
 * 编译: gcc -O2 -o algorithm algorithm.c -lm
 * 运行: ./algorithm
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <float.h>

/* ================================================================
 * 全局配置
 * ================================================================ */

/* 排序测试规模 - 多级梯度以清晰展示O(n^2) vs O(n log n) */
const int SORT_SIZES[] = {
    10, 50, 100, 200, 500,
    1000, 2000, 3000, 5000, 8000,
    10000, 20000, 30000, 50000, 80000,
    100000, 200000, 300000, 500000, 800000,
    1000000, 2000000, 5000000, 10000000
};
const int SORT_SIZE_COUNT = 24;

/* 冒泡排序最大规模（超过则跳过，O(n^2) 太慢） */
#define BUBBLE_MAX_N  200000

/* 背包测试规模 */
const int KNAPSACK_N[] = {1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000,
                           10000, 20000, 40000, 80000, 160000, 320000};
const int KNAPSACK_N_COUNT = 15;
const int KNAPSACK_C[] = {10000, 100000, 1000000};
const int KNAPSACK_C_COUNT = 3;

/* DP算法的最大容量限制 (避免内存/时间爆炸) */
#define DP_MAX_CAPACITY   1000000
#define DP_MAX_N_TIMES_C  500000000LL  /* n*C 上限，超过则跳过DP */

/* 回溯法的最大物品数限制 */
#define BACKTRACK_MAX_N   1000

/* 蛮力法的最大物品数限制 */
#define BRUTE_FORCE_MAX_N 30

/* ================================================================
 * 工具函数
 * ================================================================ */

/* 获取当前时间（毫秒） */
double get_time_ms() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000.0 + ts.tv_nsec / 1000000.0;
}

/* 生成随机整数 [min, max] */
int rand_int(int min, int max) {
    return min + rand() % (max - min + 1);
}

/* 生成随机浮点数 [min, max]，保留两位小数 */
double rand_double_2dp(double min, double max) {
    int range = (int)((max - min) * 100 + 0.5);
    int val = rand() % (range + 1);
    return min + val / 100.0;
}

/* 复制数组 */
int* copy_int_array(const int* src, int n) {
    int* dst = (int*)malloc(n * sizeof(int));
    if (dst) memcpy(dst, src, n * sizeof(int));
    return dst;
}

/* ================================================================
 * 第一部分: 排序算法
 * ================================================================ */

/* ---- 全局比较计数器 ---- */
long long compare_count = 0;

/* 重置比较计数器 */
void reset_counter() { compare_count = 0; }

/* 比较函数，记录比较次数 */
int compare(int a, int b) {
    compare_count++;
    return a - b;
}

#define LESS(a, b)  (compare_count++, (a) < (b))
#define LEQ(a, b)   (compare_count++, (a) <= (b))

/* ---- 子问题规模记录 ---- */
typedef struct {
    int* sizes;      /* 动态数组，记录每次递归调用的子问题规模 */
    int capacity;    /* 数组容量 */
    int count;       /* 当前记录数 */
} RecursionLog;

void init_recursion_log(RecursionLog* log, int initial_capacity) {
    log->capacity = initial_capacity;
    log->count = 0;
    log->sizes = (int*)malloc(initial_capacity * sizeof(int));
}

void log_subproblem(RecursionLog* log, int size) {
    if (log->count >= log->capacity) {
        log->capacity *= 2;
        log->sizes = (int*)realloc(log->sizes, log->capacity * sizeof(int));
    }
    log->sizes[log->count++] = size;
}

void free_recursion_log(RecursionLog* log) {
    free(log->sizes);
    log->sizes = NULL;
    log->capacity = log->count = 0;
}

/* ---- 冒泡排序 ---- */
void bubble_sort(int arr[], int n) {
    for (int i = 0; i < n - 1; i++) {
        int swapped = 0;
        for (int j = 0; j < n - 1 - i; j++) {
            if (arr[j] > arr[j + 1]) {  /* 使用 > 而非 LESS 宏，因为需要实际交换判断 */
                compare_count++;
                int tmp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = tmp;
                swapped = 1;
            } else {
                compare_count++;
            }
        }
        if (!swapped) break;
    }
}

/* ---- 归并排序 ---- */
static void merge(int arr[], int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;
    int* L = (int*)malloc(n1 * sizeof(int));
    int* R = (int*)malloc(n2 * sizeof(int));
    for (int i = 0; i < n1; i++) L[i] = arr[left + i];
    for (int j = 0; j < n2; j++) R[j] = arr[mid + 1 + j];

    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            compare_count++;
            arr[k++] = L[i++];
        } else {
            compare_count++;
            arr[k++] = R[j++];
        }
    }
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];
    free(L);
    free(R);
}

static void merge_sort_recursive(int arr[], int left, int right, RecursionLog* log) {
    if (log) log_subproblem(log, right - left + 1);
    if (left < right) {
        int mid = left + (right - left) / 2;
        merge_sort_recursive(arr, left, mid, log);
        merge_sort_recursive(arr, mid + 1, right, log);
        merge(arr, left, mid, right);
    }
}

void merge_sort(int arr[], int n, RecursionLog* log) {
    if (log) init_recursion_log(log, 2 * n);
    merge_sort_recursive(arr, 0, n - 1, log);
}

/* ---- 快速排序 (以第一个元素为基准) ---- */
static int partition(int arr[], int low, int high) {
    int pivot = arr[low];
    int i = low;
    for (int j = low + 1; j <= high; j++) {
        if (arr[j] < pivot) {
            compare_count++;
            i++;
            int tmp = arr[i];
            arr[i] = arr[j];
            arr[j] = tmp;
        } else {
            compare_count++;
        }
    }
    int tmp = arr[i];
    arr[i] = arr[low];
    arr[low] = tmp;
    return i;
}

static void quick_sort_recursive(int arr[], int low, int high, RecursionLog* log) {
    int size = high - low + 1;
    if (log) log_subproblem(log, size);
    if (low < high) {
        int pi = partition(arr, low, high);
        quick_sort_recursive(arr, low, pi - 1, log);
        quick_sort_recursive(arr, pi + 1, high, log);
    }
}

void quick_sort(int arr[], int n, RecursionLog* log) {
    if (log) init_recursion_log(log, 2 * n);
    quick_sort_recursive(arr, 0, n - 1, log);
}

/* ---- 排序实验: 比较两次100个数据的执行 ---- */
void sorting_experiment_1() {
    printf("\n");
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║  实验1: 排序算法 - 100个数据两次对比                        ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n\n");

    for (int trial = 1; trial <= 2; trial++) {
        int n = 100;
        int* arr = (int*)malloc(n * sizeof(int));
        int* original = (int*)malloc(n * sizeof(int));

        printf("--- 第%d次测试 ---\n", trial);
        srand((unsigned int)(time(NULL) + trial * 1000));

        /* 生成随机数 */
        printf("生成的100个随机数: ");
        for (int i = 0; i < n; i++) {
            original[i] = rand_int(1, 1000);
            printf("%d ", original[i]);
        }
        printf("\n\n");

        /* 冒泡排序 */
        memcpy(arr, original, n * sizeof(int));
        reset_counter();
        bubble_sort(arr, n);
        printf("冒泡排序 - 比较次数: %lld\n", compare_count);

        /* 归并排序 */
        memcpy(arr, original, n * sizeof(int));
        reset_counter();
        RecursionLog log_m;
        merge_sort(arr, n, &log_m);
        printf("归并排序 - 比较次数: %lld, 递归调用次数: %d\n", compare_count, log_m.count);
        free_recursion_log(&log_m);

        /* 快速排序 */
        memcpy(arr, original, n * sizeof(int));
        reset_counter();
        RecursionLog log_q;
        quick_sort(arr, n, &log_q);
        printf("快速排序 - 比较次数: %lld, 递归调用次数: %d\n", compare_count, log_q.count);
        free_recursion_log(&log_q);

        printf("\n");
        free(arr);
        free(original);
    }
    printf("分析: 冒泡排序O(n²)的比较次数远大于归并和快速排序O(n log n)。\n");
    printf("      两次测试的比较次数可能不同，体现了输入数据等价类的概念。\n");
}

/* ---- 排序实验: 不同规模对比 ---- */
void sorting_experiment_2() {
    printf("\n");
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║  实验2: 排序算法 - 不同规模对比                            ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n\n");

    /* 打开CSV输出文件 */
    FILE* fp = fopen("sorting_results.csv", "w");
    fprintf(fp, "Algorithm,Size,Comparisons\n");

    printf("%-12s %-12s %-22s\n", "算法", "规模(n)", "比较次数");
    printf("--------------------------------------------------------------\n");

    for (int si = 0; si < SORT_SIZE_COUNT; si++) {
        int n = SORT_SIZES[si];

        /* 为超大数组分配内存 */
        int* original = (int*)malloc(n * sizeof(int));
        int* arr = (int*)malloc(n * sizeof(int));
        if (!original || !arr) {
            printf("内存分配失败: n=%d, 跳过\n", n);
            free(original); free(arr);
            continue;
        }

        printf("n=%-10d", n);

        srand(42);  /* 固定种子，保证不同算法的输入数据相同 */
        for (int i = 0; i < n; i++) {
            original[i] = rand_int(1, 10000);
        }

        /* 冒泡排序 — 大规模跳过 */
        if (n <= BUBBLE_MAX_N) {
            memcpy(arr, original, n * sizeof(int));
            reset_counter();
            bubble_sort(arr, n);
            printf("  冒泡: %-18lld", compare_count);
            fprintf(fp, "Bubble,%d,%lld\n", n, compare_count);
        } else {
            printf("  冒泡: %-18s", "(跳过)");
            fprintf(fp, "Bubble,%d,0\n", n);
        }

        /* 归并排序 */
        {
            memcpy(arr, original, n * sizeof(int));
            reset_counter();
            RecursionLog log_m;
            merge_sort(arr, n, &log_m);
            printf("  归并: %-18lld", compare_count);
            fprintf(fp, "Merge,%d,%lld\n", n, compare_count);

            /* 子问题规模仅对 n <= 500000 保存到文件（避免文件过大） */
            if (n <= 500000) {
                char fname[64];
                sprintf(fname, "merge_subproblems_n%d.csv", n);
                FILE* fsub = fopen(fname, "w");
                fprintf(fsub, "CallIndex,SubproblemSize\n");
                for (int i = 0; i < log_m.count; i++) {
                    fprintf(fsub, "%d,%d\n", i + 1, log_m.sizes[i]);
                }
                fclose(fsub);
            }
            free_recursion_log(&log_m);
        }

        /* 快速排序 */
        {
            memcpy(arr, original, n * sizeof(int));
            reset_counter();
            RecursionLog log_q;
            quick_sort(arr, n, &log_q);
            printf("  快排: %-18lld", compare_count);
            fprintf(fp, "Quick,%d,%lld\n", n, compare_count);

            if (n <= 500000) {
                char fname[64];
                sprintf(fname, "quick_subproblems_n%d.csv", n);
                FILE* fsub = fopen(fname, "w");
                fprintf(fsub, "CallIndex,SubproblemSize\n");
                for (int i = 0; i < log_q.count; i++) {
                    fprintf(fsub, "%d,%d\n", i + 1, log_q.sizes[i]);
                }
                fclose(fsub);
            }
            free_recursion_log(&log_q);
        }

        printf("\n");
        free(original);
        free(arr);
    }
    fclose(fp);
    printf("\n结果已保存到 sorting_results.csv\n");
    printf("归并/快速排序的子问题规模（n<=500000）保存在 merge/quick_subproblems_n*.csv\n");
}

/* ================================================================
 * 第二部分: 0-1背包问题
 * ================================================================ */

/* 物品结构体 */
typedef struct {
    int id;          /* 物品编号 */
    int weight;      /* 重量 (整数) */
    double value;    /* 价值 (保留两位小数) */
    double ratio;    /* 价值/重量比 */
} Item;

/* 背包解 */
typedef struct {
    int* selected;   /* selected[i] = 1 表示物品i被选中 */
    int n_selected;  /* 选中物品数量 */
    double total_value;
    int total_weight;
    int feasible;    /* 是否找到可行解 */
} KnapsackSolution;

/* 初始化解 */
void init_solution(KnapsackSolution* sol, int n) {
    sol->selected = (int*)calloc(n, sizeof(int));
    sol->n_selected = 0;
    sol->total_value = 0;
    sol->total_weight = 0;
    sol->feasible = 0;
}

void free_solution(KnapsackSolution* sol) {
    free(sol->selected);
    sol->selected = NULL;
}

void copy_solution(KnapsackSolution* dst, const KnapsackSolution* src, int n) {
    memcpy(dst->selected, src->selected, n * sizeof(int));
    dst->n_selected = src->n_selected;
    dst->total_value = src->total_value;
    dst->total_weight = src->total_weight;
    dst->feasible = src->feasible;
}

/* ---- 蛮力法: 枚举所有2^n种子集 ---- */
void knapsack_brute_force(const Item items[], int n, int C, KnapsackSolution* best) {
    init_solution(best, n);

    long long total = 1LL << n;  /* 2^n */
    for (long long mask = 0; mask < total; mask++) {
        int cur_weight = 0;
        double cur_value = 0.0;

        /* 检查每一位 */
        int feasible_flag = 1;
        for (int i = 0; i < n && feasible_flag; i++) {
            if (mask & (1LL << i)) {
                cur_weight += items[i].weight;
                if (cur_weight > C) {
                    feasible_flag = 0;
                }
                cur_value += items[i].value;
            }
        }

        if (feasible_flag && cur_value > best->total_value) {
            best->total_value = cur_value;
            best->total_weight = cur_weight;
            best->feasible = 1;
            best->n_selected = 0;
            memset(best->selected, 0, n * sizeof(int));
            for (int i = 0; i < n; i++) {
                if (mask & (1LL << i)) {
                    best->selected[i] = 1;
                    best->n_selected++;
                }
            }
        }
    }
}

/* ---- 动态规划法: 一维数组优化，O(n*C)时间，O(C)空间 ---- */
void knapsack_dp(const Item items[], int n, int C, KnapsackSolution* best) {
    init_solution(best, n);

    /* dp[c] = 容量c时能达到的最大价值 */
    double* dp = (double*)malloc((C + 1) * sizeof(double));
    /* 记录选择方案: choice[i][c] = 1 表示物品i在容量c的解中被选中 */
    char** choice = (char**)malloc(n * sizeof(char*));
    for (int i = 0; i < n; i++) {
        choice[i] = (char*)calloc(C + 1, sizeof(char));
    }

    for (int c = 0; c <= C; c++) dp[c] = 0.0;

    for (int i = 0; i < n; i++) {
        int w = items[i].weight;
        double v = items[i].value;
        for (int c = C; c >= w; c--) {
            double new_val = dp[c - w] + v;
            if (new_val > dp[c]) {
                dp[c] = new_val;
                choice[i][c] = 1;
            }
        }
    }

    /* 回溯得到选择的物品 */
    best->total_value = dp[C];
    best->feasible = 1;
    int c = C;
    for (int i = n - 1; i >= 0; i--) {
        if (choice[i][c]) {
            best->selected[i] = 1;
            best->n_selected++;
            best->total_weight += items[i].weight;
            c -= items[i].weight;
        }
    }

    /* 清理 */
    for (int i = 0; i < n; i++) free(choice[i]);
    free(choice);
    free(dp);
}

/* ---- 贪心法: 按价值/重量比降序选择 ---- */
int compare_items_by_ratio(const void* a, const void* b) {
    const Item* ia = (const Item*)a;
    const Item* ib = (const Item*)b;
    if (ia->ratio > ib->ratio) return -1;
    if (ia->ratio < ib->ratio) return 1;
    return 0;
}

void knapsack_greedy(const Item items[], int n, int C, KnapsackSolution* best) {
    init_solution(best, n);

    /* 创建副本并按ratio排序 */
    Item* sorted = (Item*)malloc(n * sizeof(Item));
    int* orig_idx = (int*)malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) {
        sorted[i] = items[i];
        orig_idx[i] = i;
    }
    qsort(sorted, n, sizeof(Item), compare_items_by_ratio);

    /* 按ratio降序依次尝试装入 */
    int cur_weight = 0;
    double cur_value = 0.0;

    for (int i = 0; i < n; i++) {
        int orig_i = sorted[i].id - 1;  /* 恢复到原始索引 (id从1开始) */
        if (cur_weight + sorted[i].weight <= C) {
            cur_weight += sorted[i].weight;
            cur_value += sorted[i].value;
            best->selected[orig_i] = 1;
            best->n_selected++;
        }
    }

    best->total_value = cur_value;
    best->total_weight = cur_weight;
    best->feasible = 1;

    free(sorted);
    free(orig_idx);
}

/* ---- 回溯法: 深度优先搜索 + 上界剪枝 ---- */
typedef struct {
    double* prefix_value;   /* 前缀价值数组 */
    int* prefix_weight;     /* 前缀重量数组 */
    int n;
    int C;
    const Item* items;
    KnapsackSolution* best;
    long long nodes_visited;
} BacktrackState;

/* 计算上界: 当前价值 + 剩余物品的分数背包上界 */
static double compute_bound(const BacktrackState* state, int i, double cur_value, int cur_weight) {
    double bound = cur_value;
    int remaining_capacity = state->C - cur_weight;

    for (int j = i; j < state->n && remaining_capacity > 0; j++) {
        if (state->items[j].weight <= remaining_capacity) {
            bound += state->items[j].value;
            remaining_capacity -= state->items[j].weight;
        } else {
            bound += state->items[j].ratio * remaining_capacity;
            break;
        }
    }
    return bound;
}

static void backtrack_recursive(BacktrackState* state, int i, double cur_value,
                                 int cur_weight, int* cur_selected) {
    state->nodes_visited++;

    /* 检查是否找到更好的解 */
    if (cur_weight <= state->C && cur_value > state->best->total_value) {
        state->best->total_value = cur_value;
        state->best->total_weight = cur_weight;
        state->best->feasible = 1;
        state->best->n_selected = 0;
        for (int j = 0; j < i; j++) {
            if (cur_selected[j]) {
                state->best->selected[j] = 1;
                state->best->n_selected++;
            }
        }
    }

    /* 到达叶子节点或超出容量 */
    if (i >= state->n || cur_weight >= state->C) return;

    /* 剪枝: 如果上界不优于当前最优解，则不继续搜索 */
    double bound = compute_bound(state, i, cur_value, cur_weight);
    if (bound <= state->best->total_value) return;

    /* 尝试选择物品i */
    if (cur_weight + state->items[i].weight <= state->C) {
        cur_selected[i] = 1;
        backtrack_recursive(state, i + 1,
                            cur_value + state->items[i].value,
                            cur_weight + state->items[i].weight,
                            cur_selected);
        cur_selected[i] = 0;
    }

    /* 尝试不选物品i */
    backtrack_recursive(state, i + 1, cur_value, cur_weight, cur_selected);
}

void knapsack_backtrack(const Item items[], int n, int C, KnapsackSolution* best) {
    init_solution(best, n);

    /* 按ratio降序排序以获得更好的上界（便于剪枝） */
    Item* sorted = (Item*)malloc(n * sizeof(Item));
    int* orig_idx = (int*)malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) {
        sorted[i] = items[i];
        orig_idx[i] = i;
    }
    qsort(sorted, n, sizeof(Item), compare_items_by_ratio);
    /* 更新id到排序后的位置 */
    for (int i = 0; i < n; i++) {
        sorted[i].id = i + 1;
    }

    BacktrackState state;
    state.n = n;
    state.C = C;
    state.items = sorted;
    state.best = best;
    state.nodes_visited = 0;

    int* cur_selected = (int*)calloc(n, sizeof(int));

    backtrack_recursive(&state, 0, 0.0, 0, cur_selected);

    /* 将排序后的选择映射回原始索引 */
    if (best->feasible) {
        int* orig_selected = (int*)calloc(n, sizeof(int));
        int orig_count = 0;
        double orig_value = best->total_value;  /* 价值不变 (物品相同) */
        int orig_weight = best->total_weight;    /* 重量不变 */
        for (int i = 0; i < n; i++) {
            if (best->selected[i]) {
                orig_selected[orig_idx[i]] = 1;
                orig_count++;
            }
        }
        memcpy(best->selected, orig_selected, n * sizeof(int));
        best->n_selected = orig_count;
        free(orig_selected);
    }

    free(cur_selected);
    free(sorted);
    free(orig_idx);
}

/* ---- 生成物品数据 ---- */
Item* generate_items(int n) {
    Item* items = (Item*)malloc(n * sizeof(Item));
    for (int i = 0; i < n; i++) {
        items[i].id = i + 1;
        items[i].weight = rand_int(1, 100);
        items[i].value = rand_double_2dp(100.0, 1000.0);
        items[i].ratio = items[i].value / items[i].weight;
    }
    return items;
}

/* ---- 打印背包解 ---- */
void print_knapsack_solution(const KnapsackSolution* sol, const Item items[], int n,
                              const char* algo_name) {
    printf("  [%s] ", algo_name);
    if (!sol->feasible) {
        printf("未找到可行解\n");
        return;
    }
    printf("总价值=%.2f, 总重量=%d, 选中物品数=%d\n", sol->total_value,
           sol->total_weight, sol->n_selected);
}

/* ---- 背包实验: 不同规模测试 ---- */
void knapsack_experiment() {
    printf("\n");
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║  实验3: 0-1背包问题 - 四种算法对比                         ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n\n");

    FILE* fp = fopen("knapsack_results.csv", "w");
    fprintf(fp, "Algorithm,n_items,Capacity,TotalValue,Time_ms,Feasible\n");

    FILE* fp_detail = fopen("knapsack_detail.csv", "w");
    fprintf(fp_detail, "Algorithm,n_items,Capacity,TotalValue,TotalWeight,NumSelected,Time_ms,Feasible\n");

    srand(100);  /* 固定种子以保证可重复性 */

    for (int ci = 0; ci < KNAPSACK_C_COUNT; ci++) {
        int C = KNAPSACK_C[ci];
        printf("━━━ 背包容量 C = %d ━━━\n\n", C);
        printf("%-12s %-8s %-14s %-14s %-10s\n",
               "算法", "n", "总价值", "时间(ms)", "状态");
        printf("--------------------------------------------------------------\n");

        for (int ni = 0; ni < KNAPSACK_N_COUNT; ni++) {
            int n = KNAPSACK_N[ni];
            Item* items = generate_items(n);

            printf("n=%d:\n", n);

            /* --- 蛮力法 (仅小规模) --- */
            if (n <= BRUTE_FORCE_MAX_N) {
                KnapsackSolution sol_bf;
                double t1 = get_time_ms();
                knapsack_brute_force(items, n, C, &sol_bf);
                double t2 = get_time_ms();
                double elapsed = t2 - t1;
                printf("  %-10s %-14.2f %-14.3f %s\n",
                       "蛮力法", sol_bf.total_value, elapsed,
                       sol_bf.feasible ? "✓" : "✗");
                fprintf(fp, "BruteForce,%d,%d,%.2f,%.3f,%d\n",
                        n, C, sol_bf.total_value, elapsed, sol_bf.feasible);
                fprintf(fp_detail, "BruteForce,%d,%d,%.2f,%d,%d,%.3f,%d\n",
                        n, C, sol_bf.total_value, sol_bf.total_weight,
                        sol_bf.n_selected, elapsed, sol_bf.feasible);
                free_solution(&sol_bf);
            } else {
                printf("  %-10s (跳过: n=%d > %d)\n", "蛮力法", n, BRUTE_FORCE_MAX_N);
                fprintf(fp, "BruteForce,%d,%d,0,0,0\n", n, C);
            }

            /* --- 动态规划法 (检查可行性) --- */
            long long n_times_c = (long long)n * C;
            if (n_times_c <= DP_MAX_N_TIMES_C && C <= DP_MAX_CAPACITY) {
                KnapsackSolution sol_dp;
                double t1 = get_time_ms();
                knapsack_dp(items, n, C, &sol_dp);
                double t2 = get_time_ms();
                double elapsed = t2 - t1;
                printf("  %-10s %-14.2f %-14.3f %s\n",
                       "动态规划", sol_dp.total_value, elapsed,
                       sol_dp.feasible ? "✓" : "✗");
                fprintf(fp, "DP,%d,%d,%.2f,%.3f,%d\n",
                        n, C, sol_dp.total_value, elapsed, sol_dp.feasible);
                fprintf(fp_detail, "DP,%d,%d,%.2f,%d,%d,%.3f,%d\n",
                        n, C, sol_dp.total_value, sol_dp.total_weight,
                        sol_dp.n_selected, elapsed, sol_dp.feasible);

                /* 对于n=1000，输出详细选择信息 */
                if (n == 1000) {
                    FILE* f1000 = fopen("knapsack_1000_items.csv", "w");
                    fprintf(f1000, "ItemID,Weight,Value,ValuePerWeight,Selected\n");
                    for (int i = 0; i < n; i++) {
                        fprintf(f1000, "%d,%d,%.2f,%.4f,%d\n",
                                items[i].id, items[i].weight, items[i].value,
                                items[i].ratio, sol_dp.selected[i]);
                    }
                    fclose(f1000);
                    printf("    -> 1000个物品的详细信息已导出到 knapsack_1000_items.csv\n");
                }
                free_solution(&sol_dp);
            } else {
                printf("  %-10s (跳过: n*C=%lld > %lld)\n", "动态规划", n_times_c, DP_MAX_N_TIMES_C);
                fprintf(fp, "DP,%d,%d,0,0,0\n", n, C);
            }

            /* --- 贪心法 --- */
            {
                KnapsackSolution sol_gr;
                double t1 = get_time_ms();
                knapsack_greedy(items, n, C, &sol_gr);
                double t2 = get_time_ms();
                double elapsed = t2 - t1;
                printf("  %-10s %-14.2f %-14.3f %s\n",
                       "贪心法", sol_gr.total_value, elapsed,
                       sol_gr.feasible ? "✓" : "✗");
                fprintf(fp, "Greedy,%d,%d,%.2f,%.3f,%d\n",
                        n, C, sol_gr.total_value, elapsed, sol_gr.feasible);
                fprintf(fp_detail, "Greedy,%d,%d,%.2f,%d,%d,%.3f,%d\n",
                        n, C, sol_gr.total_value, sol_gr.total_weight,
                        sol_gr.n_selected, elapsed, sol_gr.feasible);
                free_solution(&sol_gr);
            }

            /* --- 回溯法 (仅中等规模) --- */
            if (n <= BACKTRACK_MAX_N) {
                KnapsackSolution sol_bt;
                double t1 = get_time_ms();
                knapsack_backtrack(items, n, C, &sol_bt);
                double t2 = get_time_ms();
                double elapsed = t2 - t1;
                printf("  %-10s %-14.2f %-14.3f %s\n",
                       "回溯法", sol_bt.total_value, elapsed,
                       sol_bt.feasible ? "✓" : "✗");
                fprintf(fp, "Backtrack,%d,%d,%.2f,%.3f,%d\n",
                        n, C, sol_bt.total_value, elapsed, sol_bt.feasible);
                fprintf(fp_detail, "Backtrack,%d,%d,%.2f,%d,%d,%.3f,%d\n",
                        n, C, sol_bt.total_value, sol_bt.total_weight,
                        sol_bt.n_selected, elapsed, sol_bt.feasible);
                free_solution(&sol_bt);
            } else {
                printf("  %-10s (跳过: n=%d > %d)\n", "回溯法", n, BACKTRACK_MAX_N);
                fprintf(fp, "Backtrack,%d,%d,0,0,0\n", n, C);
            }

            printf("\n");
            free(items);
        }
    }
    fclose(fp);
    fclose(fp_detail);
    printf("结果已保存到 knapsack_results.csv 和 knapsack_detail.csv\n");
}

/* ================================================================
 * 主函数
 * ================================================================ */

int main() {
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║     算法设计与分析 - 课外实验作业                           ║\n");
    printf("║     云南大学信息学院 2026年春季学期                        ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n");

    /* ---- 实验1: 排序算法 ---- */
    sorting_experiment_1();   /* 100个数据两次对比 */
    sorting_experiment_2();   /* 不同规模对比 */

    /* ---- 实验2: 0-1背包问题 ---- */
    knapsack_experiment();

    printf("\n═══════════════════════════════════════════════════════════════\n");
    printf("所有实验完成! 输出文件:\n");
    printf("  - sorting_results.csv           排序算法比较次数\n");
    printf("  - merge_subproblems_n*.csv      归并排序子问题规模\n");
    printf("  - quick_subproblems_n*.csv      快速排序子问题规模\n");
    printf("  - knapsack_results.csv          背包算法结果汇总\n");
    printf("  - knapsack_detail.csv           背包算法详细结果\n");
    printf("  - knapsack_1000_items.csv       1000个物品的详细信息\n");
    printf("═══════════════════════════════════════════════════════════════\n");

    return 0;
}
