#include <ncurses.h>
#include <dirent.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <sys/resource.h>

#include "ui.h"

// ====================== HELPER : DEMANDER UN PID ======================

static pid_t ask_pid_from_user(const char *prompt) {
    char buf[32];

    // On passe en mode bloquant + echo pour saisir du texte
    nodelay(stdscr, FALSE);
    echo();

    // Ligne de saisie en bas de l'écran
    mvprintw(LINES - 1, 0, "%s", prompt);
    clrtoeol();
    move(LINES - 1, (int)strlen(prompt));
    refresh();

    if (getnstr(buf, sizeof(buf) - 1) == ERR) {
        noecho();
        nodelay(stdscr, TRUE);
        return -1;
    }

    noecho();
    nodelay(stdscr, TRUE);

    int pid = atoi(buf);
    if (pid <= 0) return -1;
    return (pid_t)pid;
}

// ====================== HELPER : MESSAGE TEMPORAIRE ======================

static void show_status_message(const char *msg) {
    // On affiche au dessus de la ligne de commande
    mvprintw(LINES - 2, 0, "%s", msg);
    clrtoeol();
    refresh();
    // petite pause pour laisser le temps de lire
    usleep(800000); // 0,8 s
}

// ====================== F9 : KILL ======================

static void kill_pid_interactive(void) {
    pid_t pid = ask_pid_from_user("PID à tuer (SIGKILL) : ");
    if (pid <= 0) {
        show_status_message("Aucun PID valide saisi.");
        return;
    }

    if (kill(pid, SIGKILL) == 0) {
        char buf[128];
        snprintf(buf, sizeof(buf),
                 "Processus %d tué avec succès (SIGKILL).", pid);
        show_status_message(buf);
    } else {
        char buf[128];
        snprintf(buf, sizeof(buf),
                 "kill(%d) a échoué : %s", pid, strerror(errno));
        show_status_message(buf);
    }
}

// ====================== F1 : AIDE (ADAPTATION fenetre_aide) ======================

static void show_help_window(void) {
    nodelay(stdscr, FALSE); // on veut bloquer sur getch()

    clear();
    int row = 0;

    mvprintw(row++, 0, "Aide (inspiré de htop)");
    row++;

    mvprintw(row++, 0, "CPU usage bar: [low/normal/kernel/guest used%%]");
    mvprintw(row++, 0, "Memory bar:    [used/shared/compressed/buffers/cache used/total]");
    mvprintw(row++, 0, "Swap bar:      [used/cache/frontswap used/total]");
    row++;

    mvprintw(row++, 0, "Process state: R=running, S=sleeping, t=traced/stopped, Z=zombie, D=disk sleep");
    row++;

    mvprintw(row++, 0, "#          : hide/show header meters");
    mvprintw(row++, 0, "Tab        : switch to next screen tab");
    mvprintw(row++, 0, "Arrows     : scroll process list");
    mvprintw(row++, 0, "Digits     : incremental PID search");
    mvprintw(row++, 0, "F3 /       : incremental name search");
    mvprintw(row++, 0, "F4 \\\\      : incremental name filtering");
    mvprintw(row++, 0, "F5 t       : tree view");
    row++;

    mvprintw(row++, 0, "F7 ]       : higher priority (root only)");
    mvprintw(row++, 0, "F8 [       : lower priority (+ nice)");
    mvprintw(row++, 0, "F9 k       : kill process (SIGKILL)");
    mvprintw(row++, 0, "F10 q / q  : quitter le programme");
    row++;

    mvprintw(row++, 0, "F1 h ?     : afficher cette aide");
    row++;

    mvprintw(row++, 0, "Appuyez sur n'importe quelle touche pour revenir.");
    refresh();

    getch(); // on attend une touche
    nodelay(stdscr, TRUE);
}

// ====================== TABLEAU DES PROCESS ======================
// (copie de ton code existant)

static void draw_process_table(void) {
    DIR *dir = opendir("/proc");
    struct dirent *entry;
    int row = 2;

    if (!dir) {
        mvprintw(0, 0, "Impossible d'ouvrir /proc");
        return;
    }

    long clock_ticks = sysconf(_SC_CLK_TCK);

    // Uptime du système (lu une seule fois)
    double uptime_system = 0.0;
    {
        FILE *upt = fopen("/proc/uptime", "r");
        if (upt) {
            fscanf(upt, "%lf", &uptime_system);
            fclose(upt);
        }
    }

    // Mémoire totale (kB) lue une seule fois
    long mem_total = 1;
    {
        FILE *mf = fopen("/proc/meminfo", "r");
        char buf[256];
        if (mf) {
            while (fgets(buf, sizeof(buf), mf)) {
                if (sscanf(buf, "MemTotal: %ld kB", &mem_total) == 1)
                    break;
            }
            fclose(mf);
        }
    }

    while ((entry = readdir(dir)) != NULL && row < LINES - 2) {

        if (!isdigit((unsigned char)entry->d_name[0]))
            continue;

        pid_t pid = atoi(entry->d_name);
        char path[256];

        // --------- Infos UID/GID via stat ----------
        struct stat st;
        snprintf(path, sizeof(path), "/proc/%d", pid);

        if (stat(path, &st) == -1)
            continue;

        struct passwd *pw = getpwuid(st.st_uid);
        const char *user = pw ? pw->pw_name : "?";
        gid_t gid = st.st_gid;

        // --------- Nom du processus (NAME) ---------
        char name[128] = "?";
        snprintf(path, sizeof(path), "/proc/%d/comm", pid);
        FILE *f = fopen(path, "r");
        if (f) {
            if (fgets(name, sizeof(name), f)) {
                name[strcspn(name, "\n")] = 0;  // enlever le \n
            }
            fclose(f);
        }

        // --------- Lecture de /proc/<pid>/stat pour état, PPID, CPU, RSS ---------
        snprintf(path, sizeof(path), "/proc/%d/stat", pid);
        f = fopen(path, "r");
        if (!f)
            continue;

        char state;
        int ppid;
        long utime, stime, starttime, vsize, rss;

        fscanf(f,
               "%*d %*s %c %d "
               "%*d %*d %*d %*d %*u %*u %*u %*u "
               "%ld %ld %*d %*d %*d %*d %*d %ld %ld %ld",
               &state, &ppid,
               &utime, &stime, &starttime, &vsize, &rss);

        fclose(f);

        // --------- Calcul CPU% ----------
        double total_time = (utime + stime) / (double) clock_ticks;
        double seconds = uptime_system - (starttime / (double)clock_ticks);
        if (seconds < 0)
            seconds = 0;
        double cpu_percent = (seconds > 0) ? 100.0 * (total_time / seconds) : 0.0;

        // --------- Calcul MEM% ----------
        double mem_percent = 0.0;
        if (mem_total > 0) {
            // rss est en pages, on suppose page = 4kB -> rss*4 ~ kB
            mem_percent = 100.0 * ((rss * 4) / (double) mem_total);
        }

        // --------- Affichage avec largeurs fixes ----------
        // PID(6) USER(8) S(1) CPU%(7) MEM%(7) PPID(6) GID(6) NAME(30 max) UPTIME(10)
        mvprintw(row, 0,
                 "%6d %-8.8s %1c %7.2f %7.2f %6d %6d %-30.30s %10.1f",
                 pid, user, state,
                 cpu_percent, mem_percent,
                 ppid, (int)gid, name,
                 seconds);

        row++;
    }

    closedir(dir);
}

// ====================== BOUCLE PRINCIPALE TUI ======================

void run_tui(void) {
    initscr();
    cbreak();
    noecho();
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);
    flushinp();

    int ch;

    while (1) {
        ch = getch();

        // Quitter : q / Q / F10
        if (ch == 'q' || ch == 'Q' || ch == KEY_F(10)) {
            flushinp();
            break;
        }

        // Aide : F1 ou h ou H ou ?
        if (ch == KEY_F(1) || ch == 'h' || ch == 'H' || ch == '?') {
            show_help_window();
        }
            
        else if (ch == KEY_F(2)) {
            show_help_window();       //////a modif pour defiler les fenetre 
        }
            
        else if (ch == KEY_F(3)) {
            show_help_window();       //////a modif pour defiler les fenetre 
        }
            
        else if (ch == KEY_F(4)) {
            rechercher_processus(const char *nom);       ////demande le char avant
        }  
            
        else if (ch == KEY_F(5)) {
            pause_processus(pid_t pid);                   ////met en pose mais demande pid avant        
        }
            
        else if (ch == KEY_F(6)) {
            arret_processus(pid_t pid);       //////a modif demande pid avant fonction deja presente
        }

        else if (ch == KEY_F(7)) {
            kill_pid_interactive();       /////////a modif demande pid avant fonction deja presente
        }

        // F9 : kill
        else if (ch == KEY_F(9)) {
            redemarrer_processus(pid_t pid);        /////////a modif demande pid avant fonction deja presente + modif entete ligne 306
        }

        clear();

        // En-tête alignée avec les mêmes largeurs que le tableau
        mvprintw(0, 0,
                 " PID    USER     S    CPU%%    MEM%%   PPID   GID       NAME                          UPTIME");
        mvhline(1, 0, '-', COLS);

        draw_process_table();

        // Ligne de bas d'écran (rappel des touches principales)
        mvprintw(LINES - 1, 0,
                 "F1/h/?: aide  |  F7/F8: nice +/-  |  F9: kill  |  F10/q: quitter");

        refresh();
        usleep(400000); // 0,4 s
    }

    endwin();
}
