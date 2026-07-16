#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_STU 100
#define FILE_NAME "student_data.dat"

// 学生数据实体定义
typedef struct {
    char id[15];
    char name[20];
    float score;
} Student;

// 全局数据管理器，统一封装数据与操作
typedef struct {
    Student data[MAX_STU];
    int count;
} StuManager;

StuManager g_manager;

// -------------------------- 工具函数层：通用输入容错封装 --------------------------
int input_int(int min, int max) {
    int num;
    while (1) {
        if (scanf("%d", &num) != 1) {
            // 清空错误输入缓冲区
            while(getchar() != '\n');
            printf("输入无效，请输入合法数字：");
            continue;
        }
        // 校验数值范围
        if (num < min || num > max) {
            printf("请输入%d~%d之间的有效数字：", min, max);
            continue;
        }
        return num;
    }
}

float input_score() {
    float score;
    while (1) {
        if (scanf("%f", &score) != 1) {
            while(getchar() != '\n');
            printf("输入无效，请重新输入成绩：");
            continue;
        }
        // 成绩业务规则校验：0-100分
        if (score < 0 || score > 100) {
            printf("成绩必须在0~100之间，请重新输入：");
            continue;
        }
        return score;
    }
}

void input_str(char *buf, int max_len, char *prompt) {
    while (1) {
        printf("%s", prompt);
        // 限制输入长度，防止缓冲区溢出
        scanf("%s", buf);
        if (strlen(buf) >= max_len) {
            printf("输入过长，最多支持%d位字符，请重新输入\n", max_len-1);
            continue;
        }
        return;
    }
}

// -------------------------- 核心业务层：数据操作封装 --------------------------
int find_stu_by_id(char *target_id) {
    for (int i = 0; i < g_manager.count; i++) {
        if (strcmp(g_manager.data[i].id, target_id) == 0) {
            return i;
        }
    }
    return -1;
}

void sort_by_score() {
    for (int i = 0; i < g_manager.count - 1; i++) {
        for (int j = 0; j < g_manager.count - 1 - i; j++) {
            if (g_manager.data[j].score < g_manager.data[j+1].score) {
                Student temp = g_manager.data[j];
                g_manager.data[j] = g_manager.data[j+1];
                g_manager.data[j+1] = temp;
            }
        }
    }
    printf("✅ 已按成绩降序排序完成\n");
}

void stat_analysis() {
    if (g_manager.count == 0) {
        printf("暂无学生数据，无法统计\n");
        return;
    }
    float total = 0, max_s = 0, min_s = 100;
    int pass_cnt = 0;
    for (int i = 0; i < g_manager.count; i++) {
        float s = g_manager.data[i].score;
        total += s;
        if (s > max_s) max_s = s;
        if (s < min_s) min_s = s;
        if (s >= 60) pass_cnt++;
    }
    printf("\n📊 成绩统计分析报告\n");
    printf("总人数：%d | 平均分：%.1f\n", g_manager.count, total/g_manager.count);
    printf("最高分：%.1f | 最低分：%.1f\n", max_s, min_s);
    printf("及格人数：%d | 及格率：%.1f%%\n", pass_cnt, pass_cnt*100.0/g_manager.count);
}

// -------------------------- 持久化层：文件读写封装 --------------------------
void load_data() {
    FILE *fp = fopen(FILE_NAME, "rb");
    if (fp == NULL) {
        printf("ℹ️  首次运行，暂无历史数据文件\n");
        g_manager.count = 0;
        return;
    }
    fread(&g_manager.count, sizeof(int), 1, fp);
    fread(g_manager.data, sizeof(Student), g_manager.count, fp);
    fclose(fp);
    printf("✅ 成功加载%d条历史学生数据\n", g_manager.count);
}

void save_data() {
    FILE *fp = fopen(FILE_NAME, "wb");
    if (fp == NULL) {
        printf("❌ 数据保存失败，请检查目录权限\n");
        return;
    }
    fwrite(&g_manager.count, sizeof(int), 1, fp);
    fwrite(g_manager.data, sizeof(Student), g_manager.count, fp);
    fclose(fp);
}

// -------------------------- 功能入口层：业务操作实现 --------------------------
void add_stu() {
    if (g_manager.count >= MAX_STU) {
        printf("❌ 学生人数已达上限%d，无法添加\n", MAX_STU);
        return;
    }
    Student new_stu;
    input_str(new_stu.id, 15, "请输入学号：");
    if (find_stu_by_id(new_stu.id) != -1) {
        printf("❌ 该学号已存在，请勿重复添加\n");
        return;
    }
    input_str(new_stu.name, 20, "请输入姓名：");
    printf("请输入成绩：");
    new_stu.score = input_score();
    g_manager.data[g_manager.count++] = new_stu;
    printf("✅ 学生信息添加成功\n");
    save_data();
}

void del_stu() {
    char target_id[15];
    input_str(target_id, 15, "请输入要删除的学号：");
    int idx = find_stu_by_id(target_id);
    if (idx == -1) {
        printf("❌ 未找到该学生\n");
        return;
    }
    // 后续数据前移覆盖待删除元素
    for (int i = idx; i < g_manager.count - 1; i++) {
        g_manager.data[i] = g_manager.data[i+1];
    }
    g_manager.count--;
    printf("✅ 学生信息删除成功\n");
    save_data();
}

void modify_stu() {
    char target_id[15];
    input_str(target_id, 15, "请输入要修改的学号：");
    int idx = find_stu_by_id(target_id);
    if (idx == -1) {
        printf("❌ 未找到该学生\n");
        return;
    }
    printf("当前信息：学号=%s 姓名=%s 成绩=%.1f\n",
           g_manager.data[idx].id, g_manager.data[idx].name, g_manager.data[idx].score);
    input_str(g_manager.data[idx].name, 20, "请输入新姓名：");
    printf("请输入新成绩：");
    g_manager.data[idx].score = input_score();
    printf("✅ 学生信息修改成功\n");
    save_data();
}

void query_stu() {
    char target_id[15];
    input_str(target_id, 15, "请输入要查询的学号：");
    int idx = find_stu_by_id(target_id);
    if (idx == -1) {
        printf("❌ 未找到该学生\n");
        return;
    }
    printf("🔍 查询结果：学号=%s | 姓名=%s | 成绩=%.1f\n",
           g_manager.data[idx].id, g_manager.data[idx].name, g_manager.data[idx].score);
}

void show_all() {
    if (g_manager.count == 0) {
        printf("暂无学生数据\n");
        return;
    }
    printf("\n==== 全部学生列表 ====\n");
    for (int i = 0; i < g_manager.count; i++) {
        printf("[%d] 学号：%s | 姓名：%s | 成绩：%.1f\n",
               i+1, g_manager.data[i].id, g_manager.data[i].name, g_manager.data[i].score);
    }
}

void menu() {
    printf("\n==== 学生成绩管理系统 V2.0 重构版 ====\n");
    printf("1. 添加学生信息\n2. 查询学生信息\n3. 修改学生信息\n4. 删除学生信息\n");
    printf("5. 按成绩降序排序\n6. 显示全部学生\n7. 成绩统计分析\n8. 退出系统\n");
    printf("请输入操作序号：");
}

int main() {
    load_data();
    int op;
    while (1) {
        menu();
        op = input_int(1, 8);
        switch (op) {
            case 1: add_stu(); break;
            case 2: query_stu(); break;
            case 3: modify_stu(); break;
            case 4: del_stu(); break;
            case 5: sort_by_score(); break;
            case 6: show_all(); break;
            case 7: stat_analysis(); break;
            case 8: save_data(); printf("👋 数据已保存，程序正常退出\n"); return 0;
            default: printf("无效操作\n");
        }
    }
    return 0;
}
