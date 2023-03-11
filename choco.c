#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include "ini.h"

#define INI_FILENAME "./choco.ini"

#define NB_DISABLE 1
#define NB_ENABLE  0

#define GAMECOUNT 7

char* games[GAMECOUNT] = { "doom", "doom2", "heretic", "hexen", "strife", "plutonia", "tnt" };
char* commands[GAMECOUNT] = {
    "doom",
    "chocolcate-doom -file doom2.wad",
    "heretic",
    "hexen",
    "strife",
    "chocolcate-doom -file plutonia.wad",
    "chocolcate-doom -file tnt.wad",
};

char* doom_figlet[] = {
    " :::::::-.      ...         ...     .        :   ",
    "  ;;,   `';, .;;;;;;;.   .;;;;;;;.  ;;,.    ;;;  ",
    " `[[     [[,[[     \\[[,,[[     \\[[,[[[[, ,[[[[, ",
    "  $$,    $$$$$,     $$$$$$,     $$$$$$$$$$$\"$$$ ",
    "  888_,o8P'\"888,_ _,88P\"888,_ _,88P888 Y88\" 888o",
    "  MMMMP\"`    \"YMMMMMP\"   \"YMMMMMP\" MMM  M'  \"MMM",
};

char* doom2_figlet[] = {
    "d8888b.  .d88b.   .d88b.  .88b  d88. .d888b. ",
    "88  `8D .8P  Y8. .8P  Y8. 88'YbdP`88 VP  `8D ",
    "88   88 88    88 88    88 88  88  88    odD' ",
    "88   88 88    88 88    88 88  88  88  .88'   ",
    "88  .8D `8b  d8' `8b  d8' 88  88  88 j88.    ",
    "Y8888D'  `Y88P'   `Y88P'  YP  YP  YP 888888D ",
};

char* heretic_figlet[] = {
    "'||'  '||'                           .    ||",
    " ||    ||    ....  ... ..    ....  .||.  ...    ....  ",
    " ||''''||  .|...||  ||' '' .|...||  ||    ||  .|   '' ",
    " ||    ||  ||       ||     ||       ||    ||  ||      ",
    ".||.  .||.  '|...' .||.     '|...'  '|.' .||.  '|...' ",
};

char* hexen_figlet[] = {
    "          _______           _______  _ ",
    "|\\     /|(  ____ \\|\\     /|(  ____ \\( (    /|",
    "| )   ( || (    \\/( \\   / )| (    \\/|  \\  ( |",
    "| (___) || (__     \\ (_) / | (__    |   \\ | |",
    "|  ___  ||  __)     ) _ (  |  __)   | (\\ \\) |",
    "| (   ) || (       / ( ) \\ | (      | | \\   |",
    "| )   ( || (____/\\( /   \\ )| (____/\\| )  \\  |",
    "|/     \\|(_______/|/     \\|(_______/|/    )_)",
};

char* strife_figlet[] = {
    " _______ __         __   ___ ",
    "|     __|  |_.----.|__|.'  _|.-----. ",
    "|__     |   _|   _||  ||   _||  -__| ",
    "|_______|____|__|  |__||__|  |_____| ",
};

char* plutonia_figlet[] = {
    " _______ __       __               __ ",
    "|   _   |  .--.--|  |_.-----.-----|__.---.-.",
    "|.  1   |  |  |  |   _|  _  |     |  |  _  |",
    "|.  ____|__|_____|____|_____|__|__|__|___._|",
    "|:  | ",
    "|::.| ",
    "`---' ",
};

char* tnt_figlet[] = {
    " _______ _   _ _______ ",
    "|__   __| \\ | |__   __|",
    "   | |  |  \\| |  | |",
    "   | |  | . ` |  | |",
    "   | |  | |\\  |  | |",
    "   |_|  |_| \\_|  |_|",
};

#define DOOM_ROWS (sizeof(doom_figlet) / sizeof(char*))
#define DOOM2_ROWS (sizeof(doom2_figlet) / sizeof(char*))
#define HERETIC_ROWS (sizeof(heretic_figlet) / sizeof(char*))
#define HEXEN_ROWS (sizeof(hexen_figlet) / sizeof(char*))
#define STRIFE_ROWS (sizeof(strife_figlet) / sizeof(char*))
#define PLUTONIA_ROWS (sizeof(plutonia_figlet) / sizeof(char*))
#define TNT_ROWS (sizeof(tnt_figlet) / sizeof(char*))

char** figlets[GAMECOUNT] = { doom_figlet, doom2_figlet, heretic_figlet, hexen_figlet, strife_figlet, plutonia_figlet, tnt_figlet };
bool enabled[GAMECOUNT] = { false };
int cols[GAMECOUNT] = { 0 };
int rows[GAMECOUNT] = { DOOM_ROWS, DOOM2_ROWS, HERETIC_ROWS, HEXEN_ROWS, STRIFE_ROWS, PLUTONIA_ROWS, TNT_ROWS };

char* RESET = "\033[0m";
char* HIDECURSOR ="\033[?25l";
char* SHOWCURSOR = "\033[?25h";
char* CLEARSCREEN = "\033[2J";

char* BRIGHTBLACK = "\033[30;1m";
char* BRIGHTRED = "\033[31;1m";
char* BRIGHTGREEN = "\033[32;1m";
char* BRIGHTYELLOW = "\033[33;1m";
char* BRIGHTBLUE = "\033[34;1m";
char* BRIGHTMAGENTA = "\033[35;1m";
char* BRIGHTCYAN = "\033[36;1m";
char* BRIGHTWHITE = "\033[37;1m";

char* DIMBLACK = "\033[30;2m";
char* DIMRED = "\033[31;2m";
char* DIMGREEN = "\033[32;2m";
char* DIMYELLOW = "\033[33;2m";
char* DIMBLUE = "\033[34;2m";
char* DIMMAGENTA = "\033[35;2m";
char* DIMCYAN = "\033[36;2m";
char* DIMWHITE = "\033[37;2m";

int kbhit()
{
    struct timeval tv;
    fd_set fds;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds); //STDIN_FILENO is 0
    select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
    return FD_ISSET(STDIN_FILENO, &fds);
}

void nonblock(int state)
{
    struct termios ttystate;

    //get the terminal state
    tcgetattr(STDIN_FILENO, &ttystate);

    if (state == NB_ENABLE)
    {
        //turn off canonical mode
        ttystate.c_lflag &= ~ICANON;
        ttystate.c_lflag &= ~ECHO;

        //minimum of number input read.
        ttystate.c_cc[VMIN] = 1;
    }
    else if (state == NB_DISABLE)
    {
        //turn on canonical mode
        ttystate.c_lflag |= ICANON;
        ttystate.c_lflag |= ECHO;
    }

    //set the terminal attributes.
    tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
}

static int inihandler(void* config, const char* section, const char* name, const char* value)
{
    #define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0

    for (int i = 0; i < GAMECOUNT; i++)
    {
        if (MATCH("choco", games[i]))
        {
            enabled[i] = atoi(value);
        }
    }

    return 1;
}

void init()
{
    for (int i = 0; i < GAMECOUNT; i++)
    {
        char **figlet = figlets[i];
        int l = 0;
        for (int c = 0; c < rows[i]; c++)
        {
            if (strlen(figlet[c]) > l)
            {
                l = strlen(figlet[c]);
            }
        }

        cols[i] = l;
    }

    if (ini_parse(INI_FILENAME, inihandler, NULL) < 0)
    {
        printf("Can't load '%s'\n", INI_FILENAME);
        return;
    }    

    /*for (int i = 0; i < GAMECOUNT; i++)
    {
        printf("Game: %s\n", games[i]);
        printf("  Enabled: %d\n", enabled[i]);
        printf("  Rows/Cols: %d / %d\n", rows[i], cols[i]);
        printf("  Command: %s\n", commands[i]);
        printf("\n");
    }*/
}

void clear_screen()
{
    printf(CLEARSCREEN);
}

void show_cursor()
{
    printf(SHOWCURSOR);
}

void hide_cursor()
{
    printf(HIDECURSOR);
}

void move_cursor(int row, int col)
{
    printf("\033[%d;%dH", row, col);
}

void draw_figlet(int game, int col, int row, char* color)
{
    char **figlet = figlets[game];
    for (int r = 0; r < rows[game]; r++)
    {
        move_cursor(row + r, col);
        printf(color);
        printf(figlet[r]);
        printf(RESET);
    }

    fflush(stdout);
}

int loop()
{
    char c;
    int i = 0;
    int previous = 0;

    nonblock(NB_ENABLE);
    hide_cursor();
    fflush(stdout);
    while (1)
    {
        usleep(1);
        i = kbhit();

        if (i != 0)
        {
            c = fgetc(stdin);

            printf("%d \n", (int)c);

            if (c == 'q' || c == 'Q')
            {
                break;
            }

            printf("%c\n", c);
        }
    }

    nonblock(NB_DISABLE);
    show_cursor();
    return 0;
}

int main(void)
{
    init();
    clear_screen();

    draw_figlet(0, 5, 2, BRIGHTRED);
    draw_figlet(1, 60, 6, BRIGHTYELLOW);
    draw_figlet(2, 10, 12, BRIGHTBLUE);
    draw_figlet(3, 67, 14, BRIGHTMAGENTA);
    draw_figlet(4, 9, 19, BRIGHTCYAN);
    draw_figlet(5, 60, 23, BRIGHTGREEN);
    draw_figlet(6, 30, 25, BRIGHTWHITE);

    int selection = loop();

    if (selection > 0)
    {
        clear_screen();
        draw_figlet(selection - 1, 5, 6, "");
    }

    printf("0 %sBLACK%s   - %sBLACK%s\n", BRIGHTBLACK, RESET, DIMBLACK, RESET);
    printf("1 %sRED%s     - %sRED%s\n", BRIGHTRED, RESET, DIMRED, RESET);
    printf("2 %sGREEN%s   - %sGREEN%s\n", BRIGHTGREEN, RESET, DIMGREEN, RESET);
    printf("3 %sYELLOW%s  - %sYELLOW%s\n", BRIGHTYELLOW, RESET, DIMYELLOW, RESET);
    printf("4 %sBLUE%s    - %sBLUE%s\n", BRIGHTBLUE, RESET, DIMBLUE, RESET);
    printf("5 %sMAGENTA%s - %sMAGENTA%s\n", BRIGHTMAGENTA, RESET, DIMMAGENTA, RESET);
    printf("6 %sCYAN%s    - %sCYAN%s\n", BRIGHTCYAN, RESET, DIMCYAN, RESET);
    printf("7 %sWHITE%s   - %sWHITE%s\n", BRIGHTWHITE, RESET, DIMWHITE, RESET);

    return selection;
}
