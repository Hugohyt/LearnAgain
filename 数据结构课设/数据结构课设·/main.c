//
//  main.c
//  数据结构课设·
//
//  Created by 胡永泰 on 2024/12/23.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 定义家族成员结构体，模拟树节点
typedef struct FamilyMember {
    char name[50];
    struct FamilyMember* parent;
    struct FamilyMember** children;
    int num_children;
} FamilyMember;

// 创建新的家族成员节点
FamilyMember* create_member(const char* name) {
    FamilyMember* new_member = (FamilyMember*)malloc(sizeof(FamilyMember));
    strcpy(new_member->name, name);
    new_member->parent = NULL;
    new_member->children = NULL;
    new_member->num_children = 0;
    return new_member;
}

// 添加成员，将新成员添加到指定父成员的子女列表中
void add_member(FamilyMember* parent, FamilyMember* new_member) {
    if (parent == NULL) {
        return;
    }
    parent->num_children++;
    parent->children = (FamilyMember**)realloc(parent->children, parent->num_children * sizeof(FamilyMember*));
    parent->children[parent->num_children - 1] = new_member;
    new_member->parent = parent;
}

// 删除成员，需要处理好相关的亲属关系
void delete_member(FamilyMember* member) {
    if (member == NULL) {
        return;
    }
    // 处理父节点的子女列表
    if (member->parent!= NULL) {
        for (int i = 0; i < member->parent->num_children; i++) {
            if (member->parent->children[i] == member) {
                for (int j = i; j < member->parent->num_children - 1; j++) {
                    member->parent->children[j] = member->parent->children[j + 1];
                }
                member->parent->num_children--;
                member->parent->children = (FamilyMember**)realloc(member->parent->children, member->parent->num_children * sizeof(FamilyMember*));
                break;
            }
        }
    }
    // 处理子节点的父指针指向
    for (int i = 0; i < member->num_children; i++) {
        member->children[i]->parent = NULL;
    }
    free(member);
}

// 查询双亲信息
FamilyMember* find_parents(FamilyMember* member) {
    return member->parent;
}

// 简单的先序遍历输出家谱信息（树的遍历）
void pre_order_traversal(FamilyMember* member) {
    if (member == NULL) {
        return;
    }
    printf("%s\n", member->name);
    for (int i = 0; i < member->num_children; i++) {
        pre_order_traversal(member->children[i]);
    }
}

// 用于树形输出时的缩进控制
void print_indent(int level) {
    for (int i = 0; i < level; i++) {
        printf("    ");
    }
}

// 树形输出家族树信息（递归实现）
void tree_traversal(FamilyMember* member, int level) {
    if (member == NULL) {
        return;
    }
    print_indent(level);
    printf("%s\n", member->name);
    for (int i = 0; i < member->num_children; i++) {
        tree_traversal(member->children[i], level + 1);
    }
}

// 用于计数家族成员数量的函数
void count_members(FamilyMember* member, int* count) {
    if (member == NULL) {
        return;
    }
    (*count)++;
    for (int i = 0; i < member->num_children; i++) {
        count_members(member->children[i], count);
    }
}

// 辅助函数，将成员信息按格式写入文件
void save_members(FamilyMember* member, FILE* file) {
    if (member == NULL) {
        return;
    }

    // 写入成员姓名、父母姓名、子女数量及子女名字
    fprintf(file, "%s ", member->name);

    // 如果有父母，则写入父母名字，否则写入"NULL"
    if (member->parent != NULL) {
        fprintf(file, "%s ", member->parent->name);
    } else {
        fprintf(file, "NULL ");
    }

    // 写入子女数量
    fprintf(file, "%d ", member->num_children);

    // 写入每个子女的名字
    for (int i = 0; i < member->num_children; i++) {
        fprintf(file, "%s ", member->children[i]->name);
    }

    fprintf(file, "\n");

    // 递归保存所有子女的成员信息
    for (int i = 0; i < member->num_children; i++) {
        save_members(member->children[i], file);
    }
}

// 存储家族关系到文件，完善格式来保存成员信息、父子关系以及兄弟关系等
void save_to_file(FamilyMember* root, const char* file_path) {
    FILE* file = fopen(file_path, "w");
    if (file == NULL) {
        file = fopen(file_path, "a+");
        if (file == NULL) {
            perror("Error opening file");
            return;
        }
    }

    // 先写入家族成员数量（方便后续读取时分配空间等操作）
    int member_count = 0;
    count_members(root, &member_count);
    fprintf(file, "%d\n", member_count);

    // 递归遍历树，将成员信息按特定格式写入文件
    save_members(root, file);

    fclose(file);
}


// 从文件加载家族关系构建家族树，按照存储的格式解析
FamilyMember* load_from_file(const char* file_path) {
    FILE* file = fopen(file_path, "r");
    if (file == NULL) {
        perror("Error opening file");
        return NULL;
    }

    int member_count;
    fscanf(file, "%d", &member_count); // 读取成员数量

    // 创建一个数组来保存所有的家族成员
    FamilyMember** members = (FamilyMember**)malloc(member_count * sizeof(FamilyMember*)); // 存储所有成员
    char names[member_count][50];   // 保存名字
    char parent_names[member_count][50]; // 保存父亲名字
    int num_children_arr[member_count];  // 保存子女数量
    char children_names[member_count][member_count][50]; // 保存每个成员的子女名字

    for (int i = 0; i < member_count; i++) {
        fscanf(file, "%s %s %d", names[i], parent_names[i], &num_children_arr[i]);
        
        // 读取该成员的子女名字
        for (int j = 0; j < num_children_arr[i]; j++) {
            fscanf(file, "%s", children_names[i][j]);
        }
    }

    // 关闭文件，准备建立家族成员的关系
    fclose(file);

    // 创建所有成员并初始化
    for (int i = 0; i < member_count; i++) {
        members[i] = (FamilyMember*)malloc(sizeof(FamilyMember));
        strcpy(members[i]->name, names[i]);
        members[i]->parent = NULL;
        members[i]->num_children = num_children_arr[i];
        members[i]->children = (FamilyMember**)malloc(num_children_arr[i] * sizeof(FamilyMember*));
    }

    // 现在建立父子关系
    for (int i = 0; i < member_count; i++) {
        FamilyMember* current_member = members[i];
        char* parent_name = parent_names[i];

        // 查找并设置父亲
        if (strcmp(parent_name, "NULL") != 0) {
            for (int j = 0; j < member_count; j++) {
                if (strcmp(members[j]->name, parent_name) == 0) {
                    current_member->parent = members[j];
                    break;
                }
            }
        }

        // 现在设置子女
        for (int j = 0; j < num_children_arr[i]; j++) {
            char* child_name = children_names[i][j];
            // 查找并设置每个子女
            for (int k = 0; k < member_count; k++) {
                if (strcmp(members[k]->name, child_name) == 0) {
                    current_member->children[j] = members[k];
                    break;
                }
            }
        }
    }

    // 查找根节点（没有父亲的成员）
    FamilyMember* root = NULL;
    for (int i = 0; i < member_count; i++) {
        if (members[i]->parent == NULL) {
            root = members[i];
            break;
        }
    }

    return root;
}

// 查找成员的祖先并输出
void find_ancestors(FamilyMember* member) {
    if (member == NULL) {
        printf("成员不存在，无法查找祖先\n");
        return;
    }
    int count = 0;
    FamilyMember* ptr = member;
    FamilyMember* ancestors[100];  // 假设最多100个祖先，可根据实际调整大小
    while (ptr!= NULL) {
        ancestors[count++] = ptr;
        ptr = ptr->parent;
    }
    printf("祖先信息：\n");
    for (int i = count - 1; i >= 0; i--) {
        printf("%s\n", ancestors[i]->name);
    }
}

// 查找成员的后代并输出
void find_descendants(FamilyMember* member) {
    if (member == NULL) {
        printf("成员不存在，无法查找后代\n");
        return;
    }
    for (int i = 0; i < member->num_children; i++) {
        printf("%s\n", member->children[i]->name);
        find_descendants(member->children[i]);
    }
}

// 查找指定成员的父成员，简单遍历方式（优化了错误处理）
FamilyMember* find_parent(FamilyMember* root, const char* member_name) {
    if (root == NULL) {
        return NULL;
    }
    if (strcmp(root->name, member_name) == 0) {
        return root->parent;
    }
    // 递归查找子树中的成员
    for (int i = 0; i < root->num_children; i++) {
        FamilyMember* result = find_parent(root->children[i], member_name);
        if (result!= NULL) {
            return result;
        }
    }
    return NULL;
}

FamilyMember* find_member(FamilyMember* root, const char* member_name) {
    if (root == NULL) {
        return NULL;
    }
    if (strcmp(root->name, member_name) == 0) {
        return root;
    }
    // 递归查找子树中的成员
    for (int i = 0; i < root->num_children; i++) {
        FamilyMember* result = find_member(root->children[i], member_name);
        if (result!= NULL) {
            return result;
        }
    }
    return NULL;
}




int main() {
    FamilyMember* root = create_member("祖先");
    FamilyMember* child1 = create_member("第一代子女1");
    FamilyMember* child2 = create_member("第一代子女2");
    add_member(root, child1);
    add_member(root, child2);

    FamilyMember* grandchild1 = create_member("第二代子女1");
    add_member(child1, grandchild1);

    char file_path[100] = "./output/family_tree.txt";  // 默认文件名，可根据需要修改
    int choice;
    while (1) {
        printf("1. 添加成员\n");
        printf("2. 删除成员\n");
        printf("3. 查询成员的双亲信息\n");
        printf("4. 输出家谱信息（先序遍历）\n");
        printf("5. 存储家族关系到文件\n");
        printf("6. 从文件加载家族关系\n");
        printf("7. 树形输出家族树\n");
        printf("8. 查找成员的祖先\n");
        printf("9. 查找成员的后代\n");
        printf("10. 退出\n");
        scanf("%d", &choice);

        switch (choice) {
        case 1: {
            char parent_name[50];
            char new_member_name[50];
            printf("请输入父成员姓名：");
            scanf("%s", parent_name);
            printf("请输入新成员姓名：");
            scanf("%s", new_member_name);
            FamilyMember* parent = find_member(root, parent_name);  // 使用优化后的查找函数
                if (parent == NULL) {
                    printf("未找到指定的父成员，请检查输入的姓名是否正确\n");
                    break;
                }
            FamilyMember* new_member = create_member(new_member_name);
            add_member(parent, new_member);
            break;
        }
        case 2: {
            char member_name[50];
            printf("请输入要删除的成员姓名：");
            scanf("%s", member_name);
            FamilyMember* member = root;  // 同样简单假设从根开始查找要删除成员，需优化
            // 找到成员并删除
            delete_member(member);
            break;
        }
        case 3: {
            char member_name[50];
            printf("请输入要查询双亲的成员姓名：");
            scanf("%s", member_name);
            FamilyMember* member = root;  // 简单从根查找成员，可优化
            FamilyMember* parents = find_parent(member, member_name);
            if (parents != NULL) {
                printf("双亲姓名：%s\n", parents->name);
            }
            else {
                printf("未找到该成员的双亲信息\n");
            }
            break;
        }
        case 4:
            pre_order_traversal(root);
            break;
        case 5:
            save_to_file(root, file_path);
            printf("家族关系已保存到文件：%s\n", file_path);
            break;
        case 6:
            root = load_from_file(file_path);
            if (root!= NULL) {
                printf("家族关系已从文件：%s 加载成功\n", file_path);
            } else {
                printf("文件加载失败");
            }
            break;
        case 7:
            tree_traversal(root, 0);
            break;
        case 8: {
            char member_name[50];
            printf("请输入要查找祖先的成员姓名：");
            scanf("%s", member_name);
            FamilyMember* member = find_member(root, member_name);  // 简单查找，可优化
            find_ancestors(member);
            break;
        }
        case 9: {
            char member_name[50];
            printf("请输入要查找后代的成员姓名：");
            scanf("%s", member_name);
            FamilyMember* member = find_member(root, member_name);  // 简单查找，可优化
            find_descendants(member);
            break;
        }
        case 10:
            return 0;
        default:
            printf("无效的选择，请重新输入\n");
        }
    }
    return 0;
}
