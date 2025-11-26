#include <ncurses.h>
#include <dirent.h>
#include <ctype.h>
#include <sys/stat.h>
#include <pwd.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "ui.h"

// Affiche le tableau des processus : PID | USER | CMD
static void draw_process_table(void) {
    DIR *dir = opendir("/proc");
    struct dirent *entry;
    int row = 1; // ligne 0 = en-tête

    if (!dir) {
        mvprintw(0, 0, "Impossible d’ouvrir /proc");
        return;
    }

    // En-tête du tableau
    mvprintw(0, 0, " PID      USER        CMD");
    mvhline(1, 0, '-', COLS);  // ligne de séparation
    row = 2;

    while ((entry = readdir(dir)) != NULL && row < LINES - 1) {
        if (!isdigit((unsigned char)entry->d_name[0]))
            continue;

        pid_t pid = (pid_t)atoi(entry->d_name);

        // Récupérer USER via stat + getpwuid
        char path[256];
        struct stat st;
        snprintf(path, sizeof(path), "/proc/%d", pid);

        if (stat(path, &st) == -1)
            continue;

        struct passwd *pw = getpwuid(st.st_uid);
        const char *user = pw ? pw->pw_name : "?";

        // Récupérer le nom de commande dans /proc/<pid>/comm
        snprintf(path, sizeof(path), "/proc/%d/comm", pid);
        FILE *f = fopen(path, "r");
        char cmd[64] = "?";

        if (f) {
            if (fgets(cmd, sizeof(cmd), f)) {
                // enlever le \n éventuel
                cmd[strcspn(cmd, "\n")] = '\0';
            }
            fclose(f);
        }

        // Affichage d’une ligne
        mvprintw(row, 0, "%-8d %-10s %-s", pid, user, cmd);
        row++;
    }

    closedir(dir);
}

void run_tui(void) {
    initscr();              // démarre ncurses
    cbreak();
    noecho();
    nodelay(stdscr, TRUE);  // getch() non bloquant
    keypad(stdscr, TRUE);

    int ch;

    while (1) {
        // Lecture non bloquante d’une touche
        ch = getch();
        if (ch == 'q' || ch == 'Q') {
            // plus tard : d’autres touches pour pause/kill/etc.
            break;
        }

        clear();
        draw_process_table();
        mvprintw(LINES - 1, 0, "q: quitter (actions à implémenter plus tard)");
        refresh();

        // délai entre deux rafraîchissements (0,5 s)
        usleep(500000);
    }

    endwin();  // quitte ncurses
}
