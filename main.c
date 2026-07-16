#include <stdio.h>
#include <string.h>
#define MAX_STU 100  // 最大支持100名学生
#define FILE_NAME "student_data.dat" // 数据持久化文件名

// 定义学生结构体
typedef struct {
    char id[15];    // 学号
    char name[20];  // 姓名
    float score;    // 成绩
} Student;

Student stu_arr[MAX_STU];
int stu_count = 0;

// 1. 文件读写：加载本地保存的学生数据
void load_data() {
    FILE *fp = fopen(FILE_NAME, "rb");
    if(fp == NULL) return;
    fread(&stu_count, sizeof(int), 1, fp);
    fread(stu_arr, sizeof(Student), stu_count, fp);
    fclose(fp);
}

// 文件读写：将内存数据持久化写入本地文件
void save_data() {
    FILE *fp = fopen(FILE_NAME, "wb");
    if(fp == NULL) {
        printf("保存失败！\n");
        return;
    }
    fwrite(&stu_count, sizeof(int), 1, fp);
    fwrite(stu_arr, sizeof(Student), stu_count, fp);
    fclose(fp);
    printf("数据已保存到本地文件\n");
}

// 2. 查找功能：按学号查找学生
int find_stu_by_id(char *target_id) {
    for(int i=0; i<stu_count; i++) {
        if(strcmp(stu_arr[i].id, target_id) == 0) {
            return i; // 返回学生在数组中的下标
        }
    }
    return -1;
}

// 3. 排序功能：按成绩从高到低排序
void sort_by_score() {
    for(int i=0; i<stu_count-1; i++) {
        for(int j=0; j<stu_count-1-i; j++) {
            if(stu_arr[j].score < stu_arr[j+1].score) {
                Student temp = stu_arr[j];
                stu_arr[j] = stu_arr[j+1];
                stu_arr[j+1] = temp;
            }
        }
    }
    printf("已按成绩降序排序完成！\n");
}

// 添加学生信息
void add_stu() {
    if(stu_count >= MAX_STU) {
        printf("人数已满，无法添加！\n");
        return;
    }
    Student new_stu;
    printf("请输入学号：");
    scanf("%s", new_stu.id);
    // 查重校验
    if(find_stu_by_id(new_stu.id) != -1) {
        printf("该学号已存在！\n");
        return;
    }
    printf("请输入姓名：");
    scanf("%s", new_stu.name);
    printf("请输入成绩：");
    scanf("%f", &new_stu.score);
    stu_arr[stu_count++] = new_stu;
    printf("添加成功！\n");
    save_data();
}

// 查询学生信息
void query_stu() {
    char target_id[15];
    printf("请输入要查询的学号：");
    scanf("%s", target_id);
    int index = find_stu_by_id(target_id);
    if(index == -1) {
        printf("未找到该学生！\n");
        return;
    }
    printf("查询结果：学号=%s 姓名=%s 成绩=%.1f\n", 
            stu_arr[index].id, stu_arr[index].name, stu_arr[index].score);
}

// 打印全部学生
void show_all() {
    printf("\n=== 全部学生列表 ===\n");
    for(int i=0; i<stu_count; i++) {
        printf("学号：%s | 姓名：%s | 成绩：%.1f\n",
                stu_arr[i].id, stu_arr[i].name, stu_arr[i].score);
    }
}

// 主菜单
void menu() {
    printf("\n==== 多功能学生成绩管理系统 ====\n");
    printf("1. 添加学生信息\n");
    printf("2. 按学号查询学生\n");
    printf("3. 按成绩排序\n");
    printf("4. 显示全部学生\n");
    printf("5. 退出系统\n");
    printf("请输入操作序号：");
}

int main() {
    load_data(); // 程序启动自动加载本地历史数据
    int op;
    while(1) {
        menu();
        scanf("%d", &op);
        switch(op) {
            case 1: add_stu(); break;
            case 2: query_stu(); break;
            case 3: sort_by_score(); break;
            case 4: show_all(); break;
            case 5: save_data(); printf("程序退出\n"); return 0;
            default: printf("输入无效，请重新选择！\n");
        }
    }
    return 0;
}
// 新增：菜单3 - 遍历展示所有学生信息
void showAllStudents(Student *stu_arr, int count) {
    if (count == 0) {
        printf("暂无学生记录！\n");
        return;
    }
    printf("\n=== 全部学生成绩列表 ===\n");
    printf("%-10s %-10s %-6s %-6s %-6s\n", "学号", "姓名", "科1", "科2", "科3");
    for(int i = 0; i < count; i++) {
        printf("%-10d %-10s %-6d %-6d %-6d\n", 
        stu_arr[i].id, stu_arr[i].name, 
        stu_arr[i].score1, stu_arr[i].score2, stu_arr[i].score3);
    }
}

// 新增：菜单2 - 按学号精准查询学生
void searchStudentById(Student *stu_arr, int count) {
    int target_id, find_flag = 0;
    printf("请输入要查询的学生学号：");
    scanf("%d", &target_id);
    for(int i = 0; i < count; i++) {
        if(stu_arr[i].id == target_id) {
            find_flag = 1;
            printf("查询结果：学号%d 姓名%s 成绩：%d/%d/%d\n",
            stu_arr[i].id, stu_arr[i].name,
            stu_arr[i].score1, stu_arr[i].score2, stu_arr[i].score3);
            break;
        }
    }
    if(!find_flag) printf("未找到对应学号的学生记录\n");
}

// 新增：从文件加载历史学生数据，程序启动时自动调用
int loadStudentData(Student *stu_arr) {
    FILE *fp = fopen("student.dat", "rb");
    if(fp == NULL) return 0; // 首次运行无数据文件，返回0条记录
    int count = 0;
    // 循环读取每一条学生记录
    while(fread(&stu_arr[count], sizeof(Student), 1, fp) == 1) {
        count++;
    }
    fclose(fp);
    printf("从本地文件成功加载%d条历史学生记录\n", count);
    return count;
}
// 新增：退出程序前自动保存所有数据到本地文件
void saveStudentData(Student *stu_arr, int count) {
    FILE *fp = fopen("student.dat", "wb");
    if(fp == NULL) {
        printf("数据文件写入失败！\n");
        return;
    }
    fwrite(stu_arr, sizeof(Student), count, fp);
    fclose(fp);
    printf("已成功保存%d条学生数据\n", count);
}
