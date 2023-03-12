#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <time.h>
#include <sys/ioctl.h>
#include "ini.h"

#define INI_FILENAME "./choco.ini"

#define NB_DISABLE 1
#define NB_ENABLE  0

#define GAMECOUNT 7

char* games[GAMECOUNT] = { "doom", "doom2", "heretic", "hexen", "strife", "plutonia", "tnt" };

/*
commands:
    doom
    chocolcate-doom -file doom2.wad
    heretic
    hexen
    strife
    chocolcate-doom -file plutonia.wad
    chocolcate-doom -file tnt.wad
*/

char* doom_figlet[] = {
    " :::::::-.      ...         ...     .        :   ",
    "  ;;,   `';, .;;;;;;;.   .;;;;;;;.  ;;,.    ;;;  ",
    " `[[     [[,[[     \\[[,,[[     \\[[,[[[[, ,[[[[,  ",
    "  $$,    $$$$$,     $$$$$$,     $$$$$$$$$$$\"$$$  ",
    "  888_,o8P'\"888,_ _,88P\"888,_ _,88P888 Y88\" 888o ",
    "  MMMMP\"`    \"YMMMMMP\"   \"YMMMMMP\" MMM  M'  \"MMM ",
};

char* doom2_figlet[] = {
    " d8888b.  .d88b.   .d88b.  .88b  d88. .d888b. ",
    " 88  `8D .8P  Y8. .8P  Y8. 88'YbdP`88 VP  `8D ",
    " 88   88 88    88 88    88 88  88  88    odD' ",
    " 88   88 88    88 88    88 88  88  88  .88'   ",
    " 88  .8D `8b  d8' `8b  d8' 88  88  88 j88.    ",
    " Y8888D'  `Y88P'   `Y88P'  YP  YP  YP 888888D ",
};

char* heretic_figlet[] = {
    "'||'  '||'                           .    ||          ",
    " ||    ||    ....  ... ..    ....  .||.  ...    ....  ",
    " ||''''||  .|...||  ||' '' .|...||  ||    ||  .|   '' ",
    " ||    ||  ||       ||     ||       ||    ||  ||      ",
    ".||.  .||.  '|...' .||.     '|...'  '|.' .||.  '|...' ",
};

char* hexen_figlet[] = {
    " ><<     ><<                                       ",
    " ><<     ><<                                       ",
    " ><<     ><<   ><<    ><<   ><<   ><<    ><< ><<   ",
    " ><<<<<< ><< ><   ><<   >< ><<  ><   ><<  ><<  ><< ",
    " ><<     ><<><<<<< ><<   ><    ><<<<< ><< ><<  ><< ",
    " ><<     ><<><         ><  ><< ><         ><<  ><< ",
    " ><<     ><<  ><<<<   ><<   ><<  ><<<<   ><<<  ><< "
};
    
/*
char* hexen_figlet[] = {
    "          _______           _______  _       ",
    "|\\     /|(  ____ \\|\\     /|(  ____ \\( (    /|",
    "| )   ( || (    \\/( \\   / )| (    \\/|  \\  ( |",
    "| (___) || (__     \\ (_) / | (__    |   \\ | |",
    "|  ___  ||  __)     ) _ (  |  __)   | (\\ \\) |",
    "| (   ) || (       / ( ) \\ | (      | | \\   |",
    "| )   ( || (____/\\( /   \\ )| (____/\\| )  \\  |",
    "|/     \\|(_______/|/     \\|(_______/|/    )_)",
};
*/

char* strife_figlet[] = {
    "  _______ __         __   ___         ",
    " |     __|  |_.----.|__|.'  _|.-----. ",
    " |__     |   _|   _||  ||   _||  -__| ",
    " |_______|____|__|  |__||__|  |_____| ",
};

char* plutonia_figlet[] = {
    " _______ __       __               __       ",
    "|   _   |  .--.--|  |_.-----.-----|__.---.-.",
    "|.  1   |  |  |  |   _|  _  |     |  |  _  |",
    "|.  ____|__|_____|____|_____|__|__|__|___._|",
    "|:  |                                       ",
    "|::.|                                       ",
    "`---'                                       ",
};

char* tnt_figlet[] = {
    " _______ _   _ _______ ",
    "|__   __| \\ | |__   __|",
    "   | |  |  \\| |  | |   ",
    "   | |  | . ` |  | |   ",
    "   | |  | |\\  |  | |   ",
    "   |_|  |_| \\_|  |_|   ",
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

int figlet_x[] = { 0, 55, 5, 62, 4, 55, 25 };
int figlet_y[] = { 0, 4, 10, 12, 17, 21, 23 };
int figlet_color[] = { 33, 32, 31, 34, 35, 36, 32 };
int figlet_color_bg[] = { 47, 47, 47, 47, 47, 47, 47 };
int offsetx = 0;
int offsety = 0;
int terminal_width, terminal_height;
bool highlite_bg = false;
unsigned int blink_on_delay = CLOCKS_PER_SEC / 5;
unsigned int blink_off_delay = CLOCKS_PER_SEC / 10;
int blink_step = 1;
clock_t blink_last_step_time;

int getch()
{
    int r;
    unsigned char c;
    if ((r = read(0, &c, sizeof(c))) < 0) 
    {
        return r;
    } 
    else 
    {
        return c;
    }
}

int kbhit()
{
    struct timeval tv = { 0L, 0L };
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(0, &fds);
    return select(1, &fds, NULL, NULL, &tv) > 0;
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
        if (MATCH("games", games[i]))
        {
            enabled[i] = atoi(value);
        }
    }

    if (MATCH("highlite", "bg"))
    {
        highlite_bg = atoi(value);
    }

    return 1;
}

void init()
{
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    terminal_height = w.ws_row;
    terminal_width = w.ws_col;

    printf ("lines %d\n", w.ws_row);
    printf ("columns %d\n", w.ws_col);

    // calculate the width of every figlet
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

    // calculate the offsets for centering
    int maxx = 0;
    int maxy = 0;
    for (int i = 0; i < GAMECOUNT; i++)
    {
        if (figlet_y[i] + rows[i] > maxy)
        {
            maxy = figlet_y[i] + rows[i];
        }

        if (figlet_x[i] + cols[i] > maxx)
        {
            maxx = figlet_x[i] + cols[i];
        }
    }

    offsetx = (terminal_width - maxx) / 2;
    offsety = (terminal_height - maxy) / 2;

    if (ini_parse(INI_FILENAME, inihandler, NULL) < 0)
    {
        printf("Can't load '%s'\n", INI_FILENAME);
        return;
    }    

    blink_last_step_time = clock();

    /*for (int i = 0; i < GAMECOUNT; i++)
    {
        printf("Game: %s\n", games[i]);
        printf("  Enabled: %d\n", enabled[i]);
        printf("  Rows/Cols: %d / %d\n", rows[i], cols[i]);
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
        move_cursor(row + r + 1, col + 1);
        printf(color);
        printf(figlet[r]);
        printf(RESET);
    }

    fflush(stdout);
}

void draw_figlet_centered(int game)
{
    char color[20];
    sprintf(color, "\033[%d;1m", figlet_color[game]);
    int x = (terminal_width - cols[game]) / 2;
    draw_figlet(game, x, 2, color);
    move_cursor(rows[game] + 4, 1);
}

void draw_figlets(int highlited)
{
    char color[20];
    for (int i = 0; i < GAMECOUNT; i++)
    {
        sprintf(color, "\033[%d;2m", figlet_color[i]);
        if (highlited == i)
        {
            if (highlite_bg)
            {
                sprintf(color, "\033[%d;%d;1m", figlet_color[i], figlet_color_bg[i]);
            }
            else
            {
                sprintf(color, "\033[%d;1m", figlet_color[i]);
            }
        }

        if (!enabled[i])
        {
            strcpy(color, BRIGHTBLACK);
        }

        draw_figlet(i, figlet_x[i] + offsetx, figlet_y[i] + offsety, color);
    }

    fflush(stdout);
}

void draw_figlet_blink(int game)
{
    char color[20];
    if (blink_step == 1)
    {
        sprintf(color, "\033[%d;1m", figlet_color[game]);
    }
    else
    {
        sprintf(color, "\033[%d;2m", figlet_color[game]);
    }

    draw_figlet(game, figlet_x[game] + offsetx, figlet_y[game] + offsety, color);
    fflush(stdout);
}

int next_highlited(int c, int d)
{
    int l = GAMECOUNT * 2;
    while (l > 0)
    {
        c = c + d;
        if (c < 0)
        {
            c = GAMECOUNT;
        }
        else if (c >= GAMECOUNT)
        {
            c = 0;
        }

        if (enabled[c])
        {
            break;
        }

        l -= 1;
    }

    return c;
}

void check_blink(int game)
{
    clock_t current = clock();
    if (blink_step == 1)
    {
        if ((double)(current - blink_last_step_time) > blink_on_delay)
        // if (time(0) > (blink_last_step_time + blink_on_delay))
        {
            blink_step = 0;
            draw_figlet_blink(game);
            blink_last_step_time = current;
        }
    }
    else
    {
        if ((double)(current - blink_last_step_time) > blink_off_delay)
        // if (time(0) > (blink_last_step_time + blink_off_delay))
        {
            blink_step = 1;
            draw_figlet_blink(game);
            blink_last_step_time = current;
        }
    }
}

int loop()
{
    char c;
    int i = 0;
    int previous = 0;
    int highlited = 0;   // the selected game
    int selection = 0;

    nonblock(NB_ENABLE);
    hide_cursor();
    draw_figlets(highlited);

    while (1)
    {
        usleep(1);
        i = kbhit();

        if (i != 0)
        {
            c = getch(stdin);
            if (c == 27 && previous == 27)
            {
                clear_screen();
                move_cursor(1, 1);
                break;
            }
            else if (c == 'q' || c == 'Q')
            {
                clear_screen();
                move_cursor(1, 1);
                break;
            }
            else if (c == 10)
            {
                selection = highlited + 1;
                clear_screen();
                draw_figlet_centered(highlited);
                break;
            }
            else if (c == '[' && previous == 27)
            {
                c = getch(stdin);
                switch (c)
                {
                    case 'A':   // up
                        highlited = next_highlited(highlited, -1);
                        break;
                    case 'B':   // down
                        highlited = next_highlited(highlited, 1);
                        break;
                    case 'C':   // right
                        highlited = next_highlited(highlited, 1);
                        break;
                    case 'D':   // left
                        highlited = next_highlited(highlited, -1);
                        break;
                }

                draw_figlets(highlited);
            }

            previous = c;
        }
        
        check_blink(highlited);
        sleep(0.2);
    }

    nonblock(NB_DISABLE);
    show_cursor();
    return selection;
}

int main(void)
{
    init();
    clear_screen();

    int selection = loop();
    return selection;
/*
    printf("0 %sBLACK%s   - %sBLACK%s\n", BRIGHTBLACK, RESET, DIMBLACK, RESET);
    printf("1 %sRED%s     - %sRED%s\n", BRIGHTRED, RESET, DIMRED, RESET);
    printf("2 %sGREEN%s   - %sGREEN%s\n", BRIGHTGREEN, RESET, DIMGREEN, RESET);
    printf("3 %sYELLOW%s  - %sYELLOW%s\n", BRIGHTYELLOW, RESET, DIMYELLOW, RESET);
    printf("4 %sBLUE%s    - %sBLUE%s\n", BRIGHTBLUE, RESET, DIMBLUE, RESET);
    printf("5 %sMAGENTA%s - %sMAGENTA%s\n", BRIGHTMAGENTA, RESET, DIMMAGENTA, RESET);
    printf("6 %sCYAN%s    - %sCYAN%s\n", BRIGHTCYAN, RESET, DIMCYAN, RESET);
    printf("7 %sWHITE%s   - %sWHITE%s\n", BRIGHTWHITE, RESET, DIMWHITE, RESET);
*/
}
