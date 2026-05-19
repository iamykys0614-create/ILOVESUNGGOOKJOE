#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <windows.h>

#define COLOR_RESET "\033[0m" //afdsadsfsadfsafdsfsafsdfsdasdfsfsafssaffaasfsdfsdfsafasdfsdfsdfsdfsdfsfdsfdfsdfaaaaaaaaaaaaaaaaaa

#define FONT_BLACK 30
#define FONT_WHITE 37

#define BG_WHITE 107
#define BG_YELLOW 43
#define BG_MAGENTA 105

#define MENU_COUNT 4
#define LOGO_WIDTH 106
#define LOGO_HEIGHT 8

// ========================
// 콘솔 설정
// ========================
void set_console_size(int width, int height) {
    char command[50];
    sprintf(command, "mode con: cols=%d lines=%d", width, height);
    system(command);
}

// ANSI 활성화
void enable_ansi_escape() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
}

// 한글
void set_console_korean() {
    SetConsoleOutputCP(949);
    SetConsoleCP(949);
}

// ========================
// 핵심: 화면 초기화 (system cls 금지)
// ========================
void clear_screen() {
    printf("\033[2J\033[H");      // 화면 지우기 + 커서 이동
    printf("\033[48;5;15m");      // 흰 배경
    printf("\033[30m");           // 검정 글자
}

// 커서
void move_cursor(int x, int y) {
    printf("\033[%d;%dH", y, x);
}

void hide_cursor() {
    printf("\033[?25l");
}

void show_cursor() {
    printf("\033[?25h");
}

// ========================
// 로고
// ========================
void print_logo_at(int x, int y) {

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

    for (int i = 0; i < 8; i++) {
        move_cursor(x, y + i);
        printf("\033[38;5;208m"); // 주황
        printf("%s", logo[i]);
    }

    printf(COLOR_RESET);
}

// ========================
// 메뉴
// ========================
char* menu_list[MENU_COUNT] = {
    "  1. 게임 시작    ",
    "  2. 사용 설명서  ",
    "  3. 팀 소개      ",
    "  4. 게임 종료    "
};

void draw_menu(int selected) {

    clear_screen();

    // 타이틀
    move_cursor(50, 5);
    printf("\033[48;5;13m\033[37m         교수          ");

    move_cursor(50, 6);
    printf("                       ");

    move_cursor(50, 7);
    printf("                       ");

    // 메뉴
    for (int i = 0; i < MENU_COUNT; i++) {

        move_cursor(52, 10 + (i * 2));

        if (i == selected) {
            printf("\033[48;5;226m\033[30m"); // 노랑
        }
        else {
            printf("\033[48;5;15m\033[30m");  // 흰 배경 유지
        }

        printf("%s", menu_list[i]);
    }

    // 안내
    move_cursor(50, 20);
    printf("\033[48;5;15m\033[30mW/S 이동 | Enter 선택");

    printf(COLOR_RESET);
}

// ========================
// 로고 연출
// ========================
void show_logo_splash() {

    clear_screen();
    hide_cursor();

    int logo_x = 20;
    int logo_y = 5;

    // 페이드 (단순 색 변화)
    int colors[] = { 52, 88, 94, 130, 166, 172, 178, 184, 190, 208, 214, 220, 226, 227 };

    for (int i = 0; i < 14; i++) {
        clear_screen();

        printf("\033[48;5;15m"); // 흰 배경 유지

        move_cursor(logo_x, logo_y);

        printf("\033[38;5;%dm", colors[i]);

        print_logo_at(logo_x, logo_y);

        Sleep(120);
    }

    Sleep(800);

    show_cursor();
    clear_screen();
}

// ========================
// 메인
// ========================
int main() {

    enable_ansi_escape();
    set_console_size(140, 40);
    set_console_korean();

    clear_screen();

    show_logo_splash();

    int selected = 0;
    int running = 1;

    while (running) {

        draw_menu(selected);

        char input = _getch();

        if (input == 'w' || input == 'W') {
            selected--;
            if (selected < 0) selected = MENU_COUNT - 1;
        }
        else if (input == 's' || input == 'S') {
            selected++;
            if (selected >= MENU_COUNT) selected = 0;
        }
        else if (input == 13 || input == ' ') {

            clear_screen();

            if (selected == 0) {
                printf("게임 시작!");
                _getch();
            }
            else if (selected == 1) {
                printf("사용 설명서\nW/S 이동 Enter 선택");
                _getch();
            }
            else if (selected == 2) {
                printf("팀 소개\n멋진 팀입니다");
                _getch();
            }
            else if (selected == 3) {
                running = 0;
            }

            clear_screen();
        }
    }

    clear_screen();
    printf("게임 종료\n");

    return 0;
}