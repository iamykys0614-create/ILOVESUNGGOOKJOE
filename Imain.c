/*
[통합 프로젝트] 로고/메인화면 + 교수님 연애 시뮬레이터 게임 작동 코드
- 개발 환경: Visual Studio 2022
- 콘솔 크기: 사용자 조절 가능 (최소 cols=140 lines=45 이상 권장)
- 조작 방법: 오직 방향키(↑/↓) 및 ENTER/SPACE만 사용 가능
- 연출 효과: LOVE SIMULATOR 로고 고정 + 메뉴 선택 시 핑크색(Magenta) 하이라이트 전환 + 메인 모든 요소 완벽 정중앙 정렬
*/

#define _CRT_SECURE_NO_WARNINGS 
#include <string.h>
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <windows.h>

#define COLOR_RESET "\x1b[0m"
#define FONT_BLACK 30
#define FONT_WHITE 37
#define BG_BLACK 40
#define BG_MAGENTA 105      // 메뉴 선택 시 사용할 핫핑크색 배경 코드

#define MENU_COUNT 4
#define LOGO_HEIGHT 8
#define MAIN_TITLE_HEIGHT 5  // 로고 순수 높이 (5줄)

#define GAME_UI_WIDTH 115
#define GAME_UI_HEIGHT 43

enum GAMESTATE
{
    INTRO,
    STORY1,
    CHOICE1,
    QUIZ1,
    RESULT1,
    EXITGAME
};

// 인트로 전용 초대형 로고
const char* logo[] = {
    "$$\\   $$\\  $$$$$$\\        $$$$$$$\\   $$$$$$\\  $$\\        $$$$$$\\  $$\\   $$\\  $$$$$$\\  $$$$$$$$\\ ",
    "$$$\\  $$ |$$  __$$\\       $$  __$$\\ $$  __$$\\ $$ |      $$  __$$\\ $$$\\  $$ |$$  __$$\\ $$  _____|",
    "$$$$\\ $$ |$$ /  $$ |      $$ |  $$ |$$ /  $$ |$$ |      $$ /  $$ |$$$$\\ $$ |$$ /  \\__|$$ |      ",
    "$$ $$\\$$ |$$ |  $$ |      $$$$$$$\\ |$$$$$$$$ |$$ |      $$$$$$$$ |$$ $$\\$$ |$$ |      $$$$$\\    ",
    "$$ \\$$$$ |$$ |  $$ |      $$  __$$\\ $$  __$$ |$$ |      $$  __$$ |$$ \\$$$$ |$$ |      $$  __|   ",
    "$$ |\\$$$ |$$ |  $$ |      $$ |  $$ |$$ |  $$ |$$ |      $$ |  $$ |$$ |\\$$$ |$$ |  $$\\ $$ |      ",
    "$$ | \\$$ | $$$$$$  |      $$$$$$$  |$$ |  $$ |$$$$$$$$\\ $$ |  $$ |$$ | \\$$ |\\$$$$$$  |$$$$$$$$\\ ",
    "\\__|  \\__| \\______/       \\_______/ \\__|  \\__|\\________|\\__|  \\__|\\__|  \\__| \\______/ \\________|"
};

// [정렬 최적화] 부제를 분리하고 순수 영문 아스키아트만 남겨 정중앙 정렬 정밀도 극대화
const char* main_title_logo[] = {
    "  _      ____     __  __ ____      ____ ___ __  __ _   _ _        _  _____  ___  ____  ",
    " | |    / __ \\   / / / // ___|    / ___|_ _|  \\/  | | | | |      / \\|_   _|/ _ \\|  _ \\ ",
    " | |   | |  | |  \\ \\/ /| |___     \\___ \\| || |\\/| | | | | |     / _ \\ | | | | | | |_) |",
    " | |___| |__| |   \\  / | |___|     ___) ||| |  | | |_| | |___  / ___ \\| | | |_| |  _ < ",
    " |_____|\\____/     \\/  |_____|    |____/___|_|  |_|\\___/|_____/_/   \\_\\_|  \\___/|_| \\_\\"
};

// 정중앙 정렬을 위해 좌우 공백을 제거한 깔끔한 메뉴 리스트
char* menu_list[MENU_COUNT] = {
    "1. 게임 시작",
    "2. 사용 설명서",
    "3. 팀 소개",
    "4. 게임 종료"
};

int last_width = 0;
int last_height = 0;

// ===== 유틸리티 함수 =====
void set_console_korean() { SetConsoleOutputCP(949); SetConsoleCP(949); }
void set_color(int code) { printf("\x1b[%dm", code); }
void move_cursor(int x, int y) { printf("\033[%d;%dH", y + 1, x + 1); }
void set_256_color(int c) { printf("\033[38;5;%dm", c); }
void hide_cursor() { printf("\033[?25l"); }
void show_cursor() { printf("\033[?25h"); }

int get_console_width() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    return csbi.srWindow.Right - csbi.srWindow.Left + 1;
}

int get_console_height() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    return csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
}

int get_logo_width() {
    return (int)strlen(logo[0]);
}

int get_main_title_width() {
    return (int)strlen(main_title_logo[0]);
}

void enable_ansi_escape() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
}

void clear_input_buffer() {
    FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
}

void disable_quick_edit_mode() {
    HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode = 0;
    GetConsoleMode(hInput, &mode);
    mode &= ~ENABLE_QUICK_EDIT_MODE;
    mode &= ~ENABLE_MOUSE_INPUT;
    mode |= ENABLE_EXTENDED_FLAGS;
    SetConsoleMode(hInput, mode);
}

// 한글(2바이트)과 이스케이프 문자를 정밀 연산하여 가로 텍스트를 무조건 정중앙에 찍어주는 함수
void print_center(const char* text, int y) {
    int visual_len = 0;
    int i = 0;

    while (text[i] != '\0') {
        if (text[i] == '\033') {
            while (text[i] != 'm' && text[i] != '\0') i++;
            if (text[i] == 'm') i++;
            continue;
        }
        if ((unsigned char)text[i] > 127) {
            visual_len += 2;
            i += 2; // 한글 범주 처리
        }
        else {
            visual_len += 1;
            i += 1;
        }
    }

    int x = (get_console_width() - visual_len) / 2;
    if (x < 0) x = 0;
    move_cursor(x, y);
    printf("%s", text);
}

void check_and_clear_window_resize() {
    int current_width = get_console_width();
    int current_height = get_console_height();

    if (current_width != last_width || current_height != last_height) {
        system("cls");
        last_width = current_width;
        last_height = current_height;
    }
}

// ===== 인트로 로고 스플래시 =====
void print_logo_at(int x, int y) {
    for (int i = 0; i < LOGO_HEIGHT; i++) {
        move_cursor(x, y + i);
        printf("%s", logo[i]);
    }
}

void get_logo_position(int* x, int* y) {
    *x = (get_console_width() - get_logo_width()) / 2;
    *y = (get_console_height() - LOGO_HEIGHT) / 2 - 2;
    if (*x < 0) *x = 0;
    if (*y < 0) *y = 0;
}

void show_logo_splash() {
    int fade_in[] = {
        232,233,234,235,236,237,238,239,
        240,241,242,243,244,245,246,247,
        248,249,250,251,252,253,254,255
    };

    int fade_out[] = {
        255,254,253,252,251,250,249,248,
        247,246,245,244,243,242,241,240,
        239,238,237,236,235,234,233,232
    };

    hide_cursor();

    for (int i = 0; i < 24; i++) {
        int x, y;
        get_logo_position(&x, &y);
        system("cls");
        set_256_color(fade_in[i]);
        print_logo_at(x, y);
        Sleep(25);
    }

    for (int hold = 0; hold < 8; hold++) {
        int x, y;
        get_logo_position(&x, &y);
        system("cls");
        set_256_color(255);
        print_logo_at(x, y);
        Sleep(25);
    }

    for (int i = 0; i < 24; i++) {
        int x, y;
        get_logo_position(&x, &y);
        system("cls");
        set_256_color(fade_out[i]);
        print_logo_at(x, y);
        Sleep(25);
    }

    printf(COLOR_RESET);
    show_cursor();
    system("cls");

    last_width = get_console_width();
    last_height = get_console_height();
}

// ===== 메인 메뉴 화면 관련 (모든 요소 정중앙 배치) =====
void draw_menu(int selected) {
    check_and_clear_window_resize();

    // 전체 높이 밸런스를 고려한 기준점 세팅
    int title_x = (get_console_width() - get_main_title_width()) / 2;
    int base_y = (get_console_height() - 20) / 2;
    if (title_x < 0) title_x = 0;
    if (base_y < 0) base_y = 0;

    // 1. LOVE SIMULATOR 영문 로고 정중앙 출력
    set_256_color(205);
    for (int i = 0; i < MAIN_TITLE_HEIGHT; i++) {
        move_cursor(title_x, base_y + i);
        printf("%s", main_title_logo[i]);
    }
    printf(COLOR_RESET);

    // 2. 한글 부제 정중앙 배치
    int subtitle_y = base_y + MAIN_TITLE_HEIGHT + 1;
    print_center("♥ With 이 은 석 교수님 ♥", subtitle_y);

    // 3. 메뉴 리스트 정중앙 선택 연출 출력 (좌우 여백을 동적으로 감싸 균일화)
    int menu_start_y = subtitle_y + 3;
    for (int i = 0; i < MENU_COUNT; i++) {
        char buffer[100];
        if (i == selected) {
            // 하이라이트 시 화살표 패딩을 주어 가시성 상향 및 크기 유지
            sprintf(buffer, "\033[%dm\033[%dm  ▶ %s ◀  \033[0m", BG_MAGENTA, FONT_WHITE, menu_list[i]);
        }
        else {
            sprintf(buffer, "\033[%dm\033[%dm     %s     \033[0m", BG_BLACK, FONT_WHITE, menu_list[i]);
        }
        print_center(buffer, menu_start_y + (i * 2));
    }

    // 4. 하단 조작 가이드 정중앙 출력
    int guide_y = menu_start_y + (MENU_COUNT * 2) + 2;
    set_color(BG_BLACK);
    set_color(FONT_WHITE);
    print_center("방향키(↑/↓) : 이동  |  Space/Enter : 선택", guide_y);
    printf(COLOR_RESET);
}

// ===== 인게임 UI 기준 좌표 가져오기 =====
void get_game_ui_origin(int* sx, int* sy) {
    *sx = (get_console_width() - GAME_UI_WIDTH) / 2;
    *sy = (get_console_height() - GAME_UI_HEIGHT) / 2;
    if (*sx < 0) *sx = 0;
    if (*sy < 0) *sy = 0;
}

void drawUI(int affection) {
    int sx, sy;
    get_game_ui_origin(&sx, &sy);

    // ===== 1. 호감도창 =====
    move_cursor(sx + 83, sy + 2);
    printf("┌────────────────────────────┐");
    move_cursor(sx + 83, sy + 3);
    printf("│ ♥ 호감도 : %-14d  │", affection);
    move_cursor(sx + 83, sy + 4);
    printf("└────────────────────────────┘");

    // ===== 2. 대화창 =====
    move_cursor(sx + 83, sy + 6);
    printf("┌────────────────────────────┐");
    for (int i = 7; i <= 32; i++) {
        move_cursor(sx + 83, sy + i);
        printf("│                            │");
    }
    move_cursor(sx + 83, sy + 33);
    printf("└────────────────────────────┘");

    // ===== 3. 상호작용창 =====
    move_cursor(sx, sy + 35);
    printf("┌");
    for (int i = 0; i < GAME_UI_WIDTH - 2; i++) printf("─");
    printf("┐");

    for (int i = 36; i <= 41; i++) {
        move_cursor(sx, sy + i);
        printf("│");
        for (int j = 0; j < GAME_UI_WIDTH - 2; j++) printf(" ");
        printf("│");
    }

    move_cursor(sx, sy + 42);
    printf("└");
    for (int i = 0; i < GAME_UI_WIDTH - 2; i++) printf("─");
    printf("┘");
}

// ===== 실제 게임 플레이 루프 =====
void play_game() {
    int gameState = INTRO;
    int choice = 0;
    int affection = 30;
    char answer[30] = { 0 };
    int sx, sy;

    system("cls");

    while (gameState != EXITGAME) {
        get_game_ui_origin(&sx, &sy);

        switch (gameState) {
        case INTRO:
            hide_cursor();
            move_cursor((get_console_width() - 8) / 2, get_console_height() / 2);
            printf("어느날..");
            Sleep(2000);
            system("cls");
            gameState = STORY1;
            break;

        case STORY1:
        {
            int cursor = 1;
            int key;
            hide_cursor();

            while (1) {
                check_and_clear_window_resize();
                get_game_ui_origin(&sx, &sy);
                drawUI(affection);

                // 아스키 아트 출력
                move_cursor(sx + 25, sy + 12);  printf("        /^_^\\\\");
                move_cursor(sx + 25, sy + 13);  printf("       ( o o )");
                move_cursor(sx + 25, sy + 14);  printf("        | ^ |");
                move_cursor(sx + 25, sy + 15);  printf("      /-|___|-\\\\");
                move_cursor(sx + 25, sy + 16);  printf("         | |");
                move_cursor(sx + 25, sy + 17);  printf("        /   \\\\");

                // 대화창 텍스트
                move_cursor(sx + 87, sy + 12); printf("이은석 교수님이");
                move_cursor(sx + 85, sy + 14); printf("강의실로 들어오셨다.");

                move_cursor(sx + 5, sy + 37);
                if (cursor == 1) printf("\033[33m> 1. 인사를 한다\033[0m                      ");
                else             printf("  1. 인사를 한다                      ");

                move_cursor(sx + 5, sy + 38);
                if (cursor == 2) printf("\033[33m> 2. 인사를 하지 않는다\033[0m               ");
                else             printf("  2. 인사를 하지 않는다               ");

                move_cursor(sx + 5, sy + 40);
                printf("방향키(↑/↓) 이동   ENTER 선택");

                key = _getch();

                if (key == 0 || key == 224 || key == -32) {
                    key = _getch();
                    if (key == 72) {
                        cursor--;
                        if (cursor < 1) cursor = 2;
                    }
                    else if (key == 80) {
                        cursor++;
                        if (cursor > 2) cursor = 1;
                    }
                }
                else if (key == 13) {
                    choice = cursor;
                    break;
                }
            }
        }
        system("cls");
        gameState = CHOICE1;
        break;

        case CHOICE1:
            if (choice == 1) affection += 10;
            else             affection -= 5;

            check_and_clear_window_resize();
            drawUI(affection);
            hide_cursor();

            if (choice == 1) {
                move_cursor(sx + 87, sy + 12); printf("당신은 교수님께");
                move_cursor(sx + 90, sy + 14); printf("인사를 했다.");
            }
            else {
                move_cursor(sx + 85, sy + 12); printf("당신은 인사를");
                move_cursor(sx + 87, sy + 14); printf("하지 않았다.");
            }

            Sleep(1500);
            move_cursor(sx + 87, sy + 18); printf("교수님 :");
            move_cursor(sx + 85, sy + 20); printf("\"내 이름..기억나니?\"");
            Sleep(1500);

            system("cls");
            gameState = QUIZ1;
            break;

        case QUIZ1:
            check_and_clear_window_resize();
            drawUI(affection);
            show_cursor();

            move_cursor(sx + 87, sy + 12); printf("(교수님은 기대감을 안고 바라본다");
            move_cursor(sx + 5, sy + 38);  printf("이름 입력 : ");

            scanf_s("%29s", answer, (unsigned)sizeof(answer));
            while (getchar() != '\n');

            system("cls");
            gameState = RESULT1;
            break;

        case RESULT1:
            if (strcmp(answer, "이은석") == 0) affection += 10;
            else                               affection -= 10;

            check_and_clear_window_resize();
            drawUI(affection);
            hide_cursor();

            move_cursor(sx + 85, sy + 14);
            if (strcmp(answer, "이은석") == 0)
                printf("교수님 : \"오... 기억하고 있었구나.\"");
            else
                printf("교수님 : \"내 이름도 모르다니...\"");

            move_cursor(sx + 5, sy + 39);
            printf("엔터를 누르면 메인메뉴로 돌아갑니다.");

            clear_input_buffer();
            getchar();

            system("cls");
            gameState = EXITGAME;
            break;
        }
    }
}

// ===== 메인 함수 =====
int main() {
    system("mode con cols=150 lines=46");

    enable_ansi_escape();
    disable_quick_edit_mode();
    set_console_korean();

    show_logo_splash();
    clear_input_buffer();

    int selected = 0;
    int isRunning = 1;

    while (isRunning) {
        draw_menu(selected);

        int input = _getch();

        if (input == 0 || input == 224 || input == -32) {
            input = _getch();
            if (input == 72)
                selected = (selected - 1 + MENU_COUNT) % MENU_COUNT;
            else if (input == 80)
                selected = (selected + 1) % MENU_COUNT;
        }
        else if (input == ' ' || input == 13) {
            if (selected == 0) {
                play_game();
            }
            else if (selected == 1) {
                system("cls");
                print_center("[ 사용 설명서 ]", get_console_height() / 2 - 6);
                print_center("이 게임은 ♥이은석 교수님♥과의 상호작용을 통해", get_console_height() / 2 - 3);
                print_center("다양한 선택지를 골라 교수님의 \033[31m호감도\033[0m를 올리는", get_console_height() / 2 - 1);
                print_center("교수님 연애 시뮬레이터 게임입니다.", get_console_height() / 2 + 1);
                print_center("\033[31m호감도\033[0m 수치에 따라 다양한 엔딩으로 이어집니다.", get_console_height() / 2 + 3);
                print_center("방향키(↑/↓)로 이동, Space/Enter로 선택하세요.", get_console_height() / 2 + 6);
                print_center("(아무 키나 누르면 메인 메뉴로 돌아갑니다.)", get_console_height() / 2 + 8);/22
                _getch();
                system("cls");
                last_width = 0;
            }
            else if (selected == 2) {
                system("cls");
                print_center("[팀 소개]", get_console_height() / 2 - 1);
                print_center("저희 팀은 멋진 게임을 만듭니다.", get_console_height() / 2 + 1);
                _getch();
                system("cls");
                last_width = 0;
            }
            else {
                isRunning = 0;
            }
        }
    }

    system("cls");
    print_center("게임을 종료합니다.", get_console_height() / 2);
    Sleep(1500);

    return 0;
}