#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#endif

#define DAY_LENGHT_HOURS 24
#define INVENTORY_SIZE 10
#define MAX_ITEMS 10
#define MAX_NAME_LEN 32

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

    while (getchar() != '\n'); // чистим Enter после ввода цифры в меню

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

    // Читаем файл построчно
    while (fgets(line, sizeof(line), input_file) != NULL) {
        int len = strlen(line);

        // Идем по каждому символу строки обычным циклом
        for (int i = 0; i < len; i++) {
            // Если встретили <NAME> (6 символов)
            if (line[i] == '<' && line[i + 1] == 'N' && line[i + 2] == 'A' && line[i + 3] == 'M' && line[i + 4] == 'E' && line[i + 5] == '>') {
                printf("%s", farmer_name);
                fprintf(output_file, "%s", farmer_name);
                i += 5; // перешагиваем оставшиеся 5 букв тега
            }
            // Если встретили <DAY> (5 символов)
            else if (line[i] == '<' && line[i + 1] == 'D' && line[i + 2] == 'A' && line[i + 3] == 'Y' && line[i + 4] == '>') {
                printf("%d", current_day);
                fprintf(output_file, "%d", current_day);
                i += 4; // перешагиваем оставшиеся 4 буквы тега
            }
            // Если встретили <ITEM> (6 символов)
            else if (line[i] == '<' && line[i + 1] == 'I' && line[i + 2] == 'T' && line[i + 3] == 'E' && line[i + 4] == 'M' && line[i + 5] == '>') {
                printf("%s", get_item_name(inventory[0]));
                fprintf(output_file, "%s", get_item_name(inventory[0]));
                i += 5; // перешагиваем оставшиеся 5 букв тега
            }
            // Если это обычная буква, просто печатаем её одну (%c)
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

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

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
        default:
            printf("Неизвестный пункт меню! Введите число от 0 до 9.\n");
            break;
        }
    }

    return 0;
}