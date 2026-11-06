#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#endif

#define DAY_LENGHT_HOURS 24
#define INVENTORY_SIZE 10
#define MAX_ITEMS 10
#define MAX_NAME_LEN 32

#define MIN_MAP_SIZE 5
#define MAX_MAP_SIZE 30
#define MIN_CAVE_SIZE 5
#define MAX_CAVE_SIZE 8

int inventory[INVENTORY_SIZE];
char item_names[MAX_ITEMS][MAX_NAME_LEN];
char farmer_name[MAX_NAME_LEN];
int current_day = 1;
int current_hour = 1;

void load_items() {
    FILE* file = fopen("items.txt", "r");
    if (file == NULL) {
        printf("Предупреждение: не удалось открыть файл items.txt! Загружены стандартные названия.\n");
        strcpy(item_names[0], "Пусто");
        strcpy(item_names[1], "Дерево");
        strcpy(item_names[2], "Камень");
        strcpy(item_names[3], "Семена");
        strcpy(item_names[4], "Зелье");
        strcpy(item_names[5], "Отрава");
        strcpy(item_names[6], "Лопата");
        strcpy(item_names[7], "Ведро");
        strcpy(item_names[8], "Грабли");
        strcpy(item_names[9], "Коса");
        return;
    }

    int id;
    char name[MAX_NAME_LEN];
    while (fscanf(file, "%d %31s", &id, name) == 2) {
        if (id >= 0 && id < MAX_ITEMS) {
            strcpy(item_names[id], name);
        }
    }
    fclose(file);
}

const char* get_item_name(int id) {
    if (id < 0 || id >= MAX_ITEMS) {
        return "Неизвестный предмет";
    }
    return item_names[id];
}

static int safe_decimal_input(const char* input) {
    int choice;
    printf("%s", input);
    while (scanf_s("%d", &choice) != 1) {
        printf("Некорректный ввод! Введите число: ");
        while (getchar() != '\n');
    }
    return choice;
}

void input_farmer_name() {
    printf("Введите имя вашего фермера: ");
    if (fgets(farmer_name, sizeof(farmer_name), stdin) != NULL) {
        int len = strlen(farmer_name);
        if (len > 0 && farmer_name[len - 1] == '\n') {
            farmer_name[len - 1] = '\0';
        }
    }
    if (strlen(farmer_name) == 0) {
        strcpy(farmer_name, "Фермер");
    }
}

void print_menu() {
    printf("\n=== МЕНЮ ФЕРМЕРА: %s ===\n", farmer_name);
    printf("[0] Выход\n");
    printf("[1] Посмотреть на часы\n");
    printf("[2] Промотать время (Поработать)\n");
    printf("[3] Посмотреть инвентарь\n");
    printf("[4] Положить предмет в слот\n");
    printf("[5] Выбросить предмет\n");
    printf("[6] Быстрый доступ\n");
    printf("[7] Поиск предмета в рюкзаке по названию\n");
    printf("[8] Записать состояние в дневник фермера (diary.txt)\n");
    printf("[9] Расшифровать старые записи (Вариант 4)\n");
    printf("[10] Отправиться в приключение\n");
}

void show_time() {
    printf("Текущее время: День %d, %02d:00\n", current_day, current_hour);
}

void advance_time() {
    int work_hours = safe_decimal_input("Сколько часов хотите поработать? ");
    if (work_hours < 0) {
        printf("Ошибка: количество часов не может быть отрицательным.\n");
        return;
    }
    int total = current_hour + work_hours;
    current_day += total / DAY_LENGHT_HOURS;
    current_hour = total % DAY_LENGHT_HOURS;
}

void print_inventory() {
    printf("\n--- Инвентарь (%s) ---\n", farmer_name);
    for (int i = 0; i < INVENTORY_SIZE; ++i) {
        printf("Слот %d: [%d] - %s\n", i, inventory[i], get_item_name(inventory[i]));
    }
}

void put_item_in_slot() {
    int slot = safe_decimal_input("В какой слот? ");
    if (slot < 0 || slot >= INVENTORY_SIZE) {
        printf("Ошибка: неверный индекс слота! Допустимо от 0 до %d.\n", INVENTORY_SIZE - 1);
        return;
    }
    int item_id = safe_decimal_input("Какой предмет? (ID) ");
    if (item_id < 0 || item_id >= MAX_ITEMS) {
        printf("Ошибка: такого ID предмета не существует!\n");
        return;
    }
    inventory[slot] = item_id;
    printf("В слот %d помещен предмет с ID %d (%s).\n", slot, item_id, get_item_name(item_id));
}

void remove_item_from_slot() {
    int slot = safe_decimal_input("Какой слот очистить? ");
    if (slot < 0 || slot >= INVENTORY_SIZE) {
        printf("Ошибка: неверный индекс слота! Допустимо от 0 до %d.\n", INVENTORY_SIZE - 1);
        return;
    }
    inventory[slot] = 0;
    printf("Слот %d очищен.\n", slot);
}

void quick_access() {
    int found_index = -1;

    printf("\n--- Инвентарь ДО ---\n");
    print_inventory();
    printf("\n");

    int target_id = safe_decimal_input("Введите ID предмета: ");
    for (int i = 0; i < INVENTORY_SIZE; ++i) {
        if (inventory[i] == target_id) {
            found_index = i;
            break;
        }
    }

    if (found_index == -1) {
        printf("Предмет с ID %d не найден в инвентаре!\n", target_id);
    }
    else {
        int temp = inventory[0];
        inventory[0] = inventory[found_index];
        inventory[found_index] = temp;
        printf("Предмет с ID %d перемещен в слот 0 (из слота %d).\n", target_id, found_index);
        printf("\n--- Инвентарь ПОСЛЕ ---\n");
        print_inventory();
        printf("\n");
    }
}

void search_item_by_name() {
    char target_name[MAX_NAME_LEN];
    printf("Введите название предмета для поиска: ");

    while (getchar() != '\n');

    if (fgets(target_name, sizeof(target_name), stdin) != NULL) {
        int len = strlen(target_name);
        if (len > 0 && target_name[len - 1] == '\n') {
            target_name[len - 1] = '\0';
        }
    }

    int found_id = -1;
    for (int i = 0; i < MAX_ITEMS; ++i) {
        if (strcmp(item_names[i], target_name) == 0) {
            found_id = i;
            break;
        }
    }

    if (found_id == -1) {
        printf("Такого предмета нет в каталоге!\n");
        return;
    }

    int count = 0;
    for (int i = 0; i < INVENTORY_SIZE; ++i) {
        if (inventory[i] == found_id) {
            printf("Предмет найден в слоте %d\n", i);
            count++;
        }
    }

    if (count == 0) {
        printf("В рюкзаке этого предмета сейчас нет.\n");
    }
}

void save_to_diary() {
    FILE* file = fopen("diary.txt", "a");
    if (file == NULL) {
        printf("Ошибка: не удалось открыть файл diary.txt!\n");
        return;
    }

    printf("\n=== ЗАПИСЬ В ДНЕВНИК ===\n");
    printf("Фермер: %s | День %d, %02d:00\n", farmer_name, current_day, current_hour);
    fprintf(file, "=== ЗАПИСЬ В ДНЕВНИК ===\n");
    fprintf(file, "Фермер: %s | День %d, %02d:00\n", farmer_name, current_day, current_hour);

    for (int i = 0; i < INVENTORY_SIZE; ++i) {
        printf("Слот %d: %s\n", i, get_item_name(inventory[i]));
        fprintf(file, "Слот %d: %s\n", i, get_item_name(inventory[i]));
    }
    fprintf(file, "\n");

    fclose(file);
    printf("Сохранено в diary.txt!\n");
}

void process_npc_dialogue_template() {
    FILE* input_file = fopen("input.txt", "r");
    if (input_file == NULL) {
        printf("Ошибка: не удалось открыть файл input.txt!\n");
        return;
    }

    FILE* output_file = fopen("output.txt", "w");
    if (output_file == NULL) {
        printf("Ошибка: не удалось создать файл output.txt!\n");
        fclose(input_file);
        return;
    }

    char line[256];
    printf("\n=== ТЕКСТ ПИСЬМА (output.txt) ===\n");

    while (fgets(line, sizeof(line), input_file) != NULL) {
        int len = strlen(line);

        for (int i = 0; i < len; i++) {
            if (line[i] == '<' && line[i + 1] == 'N' && line[i + 2] == 'A' && line[i + 3] == 'M' && line[i + 4] == 'E' && line[i + 5] == '>') {
                printf("%s", farmer_name);
                fprintf(output_file, "%s", farmer_name);
                i += 5;
            }
            else if (line[i] == '<' && line[i + 1] == 'D' && line[i + 2] == 'A' && line[i + 3] == 'Y' && line[i + 4] == '>') {
                printf("%d", current_day);
                fprintf(output_file, "%d", current_day);
                i += 4;
            }
            else if (line[i] == '<' && line[i + 1] == 'I' && line[i + 2] == 'T' && line[i + 3] == 'E' && line[i + 4] == 'M' && line[i + 5] == '>') {
                printf("%s", get_item_name(inventory[0]));
                fprintf(output_file, "%s", get_item_name(inventory[0]));
                i += 5;
            }
            else {
                printf("%c", line[i]);
                fprintf(output_file, "%c", line[i]);
            }
        }
    }

    fclose(input_file);
    fclose(output_file);
    printf("\nГотово! Результат записан в output.txt.\n");
}

char** allocate_map(int height, int width) {
    char** map = (char**)malloc(height * sizeof(char*));
    if (map == NULL) {
        return NULL;
    }
    for (int i = 0; i < height; ++i) {
        map[i] = (char*)malloc(width * sizeof(char));
        if (map[i] == NULL) {
            for (int j = 0; j < i; ++j) {
                free(map[j]);
            }
            free(map);
            return NULL;
        }
    }
    return map;
}

void free_map(char** map, int height) {
    if (map == NULL) {
        return;
    }
    for (int i = 0; i < height; ++i) {
        free(map[i]);
    }
    free(map);
}

void print_map(char** map, int height, int width, int player_y, int player_x) {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (y == player_y && x == player_x) {
                printf("@ ");
            }
            else {
                printf("%c ", map[y][x]);
            }
        }
        printf("\n");
    }
}

void generate_forest_map(char*** map_ptr, int* height_ptr, int* width_ptr, int* player_y, int* player_x) {
    if (*map_ptr != NULL) {
        free_map(*map_ptr, *height_ptr);
        *map_ptr = NULL;
    }

    int h = MIN_MAP_SIZE + rand() % (MAX_MAP_SIZE - MIN_MAP_SIZE + 1);
    int w = MIN_MAP_SIZE + rand() % (MAX_MAP_SIZE - MIN_MAP_SIZE + 1);
    *height_ptr = h;
    *width_ptr = w;

    *map_ptr = allocate_map(h, w);
    if (*map_ptr == NULL) {
        printf("Ошибка выделения памяти под карту леса!\n");
        return;
    }

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            if (y == 0 || y == h - 1 || x == 0 || x == w - 1) {
                (*map_ptr)[y][x] = '#';
            }
            else {
                int roll = rand() % 100;
                if (roll < 12) {
                    (*map_ptr)[y][x] = 'T';
                }
                else if (roll < 22) {
                    (*map_ptr)[y][x] = 'R';
                }
                else {
                    (*map_ptr)[y][x] = '.';
                }
            }
        }
    }

    int camp_y = 1 + rand() % (h - 2);
    int camp_x = 1 + rand() % (w - 2);
    (*map_ptr)[camp_y][camp_x] = '^';

    *player_y = camp_y;
    *player_x = camp_x;

    int cave_y = 1 + rand() % (h - 2);
    int cave_x = 1 + rand() % (w - 2);
    while (cave_y == camp_y && cave_x == camp_x) {
        cave_y = 1 + rand() % (h - 2);
        cave_x = 1 + rand() % (w - 2);
    }
    (*map_ptr)[cave_y][cave_x] = 'O';
}

void generate_cave_map(char*** map_ptr, int* height_ptr, int* width_ptr, int* player_y, int* player_x) {
    if (*map_ptr != NULL) {
        free_map(*map_ptr, *height_ptr);
        *map_ptr = NULL;
    }

    int h = MIN_CAVE_SIZE + rand() % (MAX_CAVE_SIZE - MIN_CAVE_SIZE + 1);
    int w = MIN_CAVE_SIZE + rand() % (MAX_CAVE_SIZE - MIN_CAVE_SIZE + 1);
    *height_ptr = h;
    *width_ptr = w;

    *map_ptr = allocate_map(h, w);
    if (*map_ptr == NULL) {
        printf("Ошибка выделения памяти под карту пещеры!\n");
        return;
    }

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            if (y == 0 || y == h - 1 || x == 0 || x == w - 1) {
                (*map_ptr)[y][x] = '#';
            }
            else {
                int roll = rand() % 100;
                if (roll < 25) {
                    (*map_ptr)[y][x] = 'R';
                }
                else {
                    (*map_ptr)[y][x] = '.';
                }
            }
        }
    }

    int exit_y = 1 + rand() % (h - 2);
    int exit_x = 1 + rand() % (w - 2);
    (*map_ptr)[exit_y][exit_x] = 'E';

    int py = 1 + rand() % (h - 2);
    int px = 1 + rand() % (w - 2);
    while (py == exit_y && px == exit_x) {
        py = 1 + rand() % (h - 2);
        px = 1 + rand() % (w - 2);
    }
    (*map_ptr)[py][px] = '.';
    *player_y = py;
    *player_x = px;
}

void adventure_mode() {
    char** map = NULL;
    int height = 0;
    int width = 0;
    int player_y = 0;
    int player_x = 0;
    int in_cave = 0;

    generate_forest_map(&map, &height, &width, &player_y, &player_x);
    if (map == NULL) {
        return;
    }

    printf("\nВы отправились в приключение в Лес!\n");

    while (1) {
        printf("\n");
        if (in_cave) {
            printf("Локация: Пещера (Выход: E)\n");
        }
        else {
            printf("Локация: Лес (Костёр: ^, Пещера: O)\n");
        }

        print_map(map, height, width, player_y, player_x);

        printf("Управление: W - вверх, A - влево, S - вниз, D - вправо, Q - сбежать домой: ");
        char command;
        while (scanf_s(" %c", &command, 1) != 1);

        if (command == 'q' || command == 'Q') {
            if (!in_cave && map[player_y][player_x] == '^') {
                printf("Вы безопасно вернулись домой из лагеря.\n");
                break;
            }
            else if (in_cave) {
                printf("Нельзя сбежать домой прямо из пещеры! Найдите выход (E).\n");
                continue;
            }
            else {
                printf("Сбежать домой можно только стоя на клетке лагеря (^).\n");
                continue;
            }
        }

        int target_y = player_y;
        int target_x = player_x;

        if (command == 'w' || command == 'W') target_y--;
        else if (command == 's' || command == 'S') target_y++;
        else if (command == 'a' || command == 'A') target_x--;
        else if (command == 'd' || command == 'D') target_x++;
        else {
            printf("Неизвестная команда!\n");
            continue;
        }

        if (target_y < 0 || target_y >= height || target_x < 0 || target_x >= width) {
            printf("Там непроходимая глушь!\n");
            continue;
        }

        char cell = map[target_y][target_x];
        if (cell == '#' || cell == 'T' || cell == 'R') {
            printf("Путь заблокирован препятствием!\n");
            continue;
        }

        player_y = target_y;
        player_x = target_x;

        if (!in_cave && cell == 'O') {
            printf("\nВы спустились во тьму пещеры...\n");
            generate_cave_map(&map, &height, &width, &player_y, &player_x);
            if (map == NULL) {
                return;
            }
            in_cave = 1;
        }
        else if (in_cave && cell == 'E') {
            printf("\nВы выбрались из пещеры обратно в лес!\n");
            generate_forest_map(&map, &height, &width, &player_y, &player_x);
            if (map == NULL) {
                return;
            }
            in_cave = 0;
        }
    }

    free_map(map, height);
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    srand((unsigned int)time(NULL));

    input_farmer_name();
    load_items();

    for (int i = 0; i < INVENTORY_SIZE; ++i) {
        inventory[i] = 0;
    }
    inventory[1] = 1;
    inventory[2] = 2;

    int choice = -1;

    while (1) {
        print_menu();
        choice = safe_decimal_input("Что делать дальше? ");

        switch (choice) {
        case 0:
            return 0;
        case 1:
            show_time();
            break;
        case 2:
            advance_time();
            break;
        case 3:
            print_inventory();
            break;
        case 4:
            put_item_in_slot();
            break;
        case 5:
            remove_item_from_slot();
            break;
        case 6:
            quick_access();
            break;
        case 7:
            search_item_by_name();
            break;
        case 8:
            save_to_diary();
            break;
        case 9:
            process_npc_dialogue_template();
            break;
        case 10:
            adventure_mode();
            break;
        default:
            printf("Неизвестный пункт меню! Введите число от 0 до 10.\n");
            break;
        }
    }

    return 0;
}