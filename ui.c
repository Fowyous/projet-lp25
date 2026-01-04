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
#include "process.h"

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

// ======================  KILL ======================

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
	nodelay(stdscr, FALSE); // bloquant
	keypad(stdscr, TRUE);

	const char *help_lines[] = {
		"Aide (inspire de htop)",
		"",
		"CPU usage bar: [low/normal/kernel/guest used%]",
		"Memory bar:    [used/shared/compressed/buffers/cache used/total]",
		"Swap bar:      [used/cache/frontswap used/total]",
		"",
		"Process state:",
		" R = running",
		" S = sleeping",
		" t = traced/stopped",
		" Z = zombie",
		" D = disk sleep",
		"",
		"#          : hide/show header meters",
		"Tab        : switch to next screen tab",
		"Arrows     : scroll process list",
		"Digits     : incremental PID search",
		"F3 /       : incremental name search",
		"F4 \\      : incremental name filtering",
		"F5 t       : tree view",
		"",
		"F7 ]       : higher priority (root only)",
		"F8 [       : lower priority (+ nice)",
		"F9 k       : kill process (SIGKILL)",
		"F10 q / q  : quitter le programme",
		"",
		"F1 h ?     : afficher cette aide",
		"",
		"F2         : defiler vers le bas",
		"F3         : defiler vers le haut",
		"",
		"Appuyez sur q ou ESC pour revenir."
	};

	int total_lines = sizeof(help_lines) / sizeof(help_lines[0]);
	int scroll = 0;
	int ch;

	while (1) {
		clear();

		int max_display = LINES - 2;
		for (int i = 0; i < max_display; i++) {
			int idx = scroll + i;
			if (idx >= total_lines)
				break;
			mvprintw(i, 0, "%s", help_lines[idx]);
		}

		mvprintw(LINES - 1, 0,
				"F2: bas  F3: haut  q/ESC: retour");
		refresh();

		ch = getch();

		if (ch == 'q' || ch == 27 || ch == KEY_F(10)) {
			break;
		}
		else if (ch == KEY_F(2)) {
			if (scroll < total_lines - max_display)
				scroll++;
		}
		else if (ch == KEY_F(3)) {
			if (scroll > 0)
				scroll--;
		}
	}

	nodelay(stdscr, TRUE); // retour non bloquant
}




/*ou a voir ancienne version
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
  */

// ====================== TABLEAU DES PROCESS ======================
// (copie de ton code existant)
static pid_t draw_process_table(pid_t start_pid) {
    DIR *dir = opendir("/proc");
    struct dirent *entry;
    int row = 2;
    pid_t last_pid = -1;

    if (!dir) {
        mvprintw(0, 0, "Impossible d'ouvrir /proc");
        return last_pid;
    }

    while ((entry = readdir(dir)) != NULL && row < LINES - 2) {

        if (!isdigit((unsigned char)entry->d_name[0]))
            continue;

        pid_t pid = atoi(entry->d_name);
	if (pid < start_pid)
		continue;
        proc_info_t info = get_process_info(pid);

        if (info.pid == -1)
            continue;

	last_pid = info.pid;

        // Sélection de la couleur selon l’état
        int color = 6; // default

        switch (info.state) {
            case 'R': color = 1; break; // Running
            case 'S': color = 2; break; // Sleeping
            case 'D': color = 3; break; // Disk sleep
            case 'Z': color = 4; break; // Zombie
            case 'T': color = 5; break; // Stopped
            default:  color = 6; break;
        }

        attron(COLOR_PAIR(color));

        mvprintw(row, 0,
                 "%6d %-8.8s %1c %7.2f %7.2f %6d %6d %-30.30s %10.1f",
                 info.pid,
                 info.user,
                 info.state,
                 info.cpu_percent,
                 info.mem_percent,
                 info.ppid,
                 (int)info.gid,
                 info.name,
                 info.uptime_seconds);

        attroff(COLOR_PAIR(color));

        row++;
    }

    closedir(dir);

    return last_pid;
}

// ====================== Recherche de processus ======================
void rechercher_processus(const char *nom) {
    DIR *dir;
    struct dirent *entry;
    int row = 4;

    clear();
    mvprintw(1, 2, "Resultats de recherche pour : \"%s\"", nom);
    mvprintw(2, 2, "PID     NAME");
    mvprintw(3, 2, "-----------------------------");

    dir = opendir("/proc");
    if (!dir) {
        mvprintw(row, 2, "Impossible d'ouvrir /proc");
        refresh();
        getch();
        return;
    }

    while ((entry = readdir(dir)) != NULL && row < LINES - 2) {
        if (!isdigit(entry->d_name[0]))
            continue;

        pid_t pid = atoi(entry->d_name);
        char path[256];
        char pname[64] = "?";
        FILE *f;

        snprintf(path, sizeof(path), "/proc/%d/comm", pid);
        f = fopen(path, "r");
        if (f) {
            fgets(pname, sizeof(pname), f);
            pname[strcspn(pname, "\n")] = '\0';
            fclose(f);
        }

        /* Recherche partielle (substring) */
        if (strstr(pname, nom)) {
            mvprintw(row++, 2, "%-7d %s", pid, pname);
        }
    }

    closedir(dir);

    mvprintw(LINES - 1, 2, "Appuyez sur une touche pour revenir");
    refresh();
    getch();
}


// ====================== demander processus ======================
static proc_info_t demander_processus(void) {
    proc_info_t p;
    char buf[32];

    /* Initialisation propre de la structure */
    memset(&p, 0, sizeof(proc_info_t));
    p.pid = -1;

    nodelay(stdscr, FALSE);
    echo();
    curs_set(1);

    clear();
    mvprintw(2, 2, "Entrer le PID du processus : ");
    refresh();

    getnstr(buf, sizeof(buf) - 1);

    noecho();
    curs_set(0);
    nodelay(stdscr, TRUE);

    p.pid = (pid_t)atoi(buf);

    return p;
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
    pid_t f3start_pid = 1;//le start pid de la fenetre precedente
    pid_t start_pid = 1;
    pid_t last_pid = -1;

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

        // F2 : onglet suivant
        else if (ch == KEY_F(2)) {
		f3start_pid = start_pid;
        	start_pid = last_pid;
	}

        // F3 : onglet précédent
        else if (ch == KEY_F(3)) {
        	//start_pid = 
	}

        // F4 : recherche
        else if (ch == KEY_F(4)) {
            //rechercher_processus();                                                         ///a verif
        }  

        // F5 : pause
        else if (ch == KEY_F(5)) {
                proc_info_t proc = demander_processus();
   	        if (proc.pid > 0) {
                pause_processus(proc.pid);
                proc.pid = 0;
	        }                         
        }

        // F6 : arret
        else if (ch == KEY_F(6)) {
            proc_info_t proc = demander_processus();
    	    if (proc.pid > 0) {
                arret_processus(proc.pid);
                proc.pid = 0;
	        }
        }

        // F7 : kill
        else if (ch == KEY_F(7)) {
            proc_info_t proc = demander_processus();
    	    if (proc.pid > 0) {
                kill(proc.pid, SIGKILL);
                proc.pid = 0;
	        }      
        }

        // F8 : redémare
        else if (ch == KEY_F(8)) {
            proc_info_t proc = demander_processus();
	        if (proc.pid > 0) {
                redemarrer_processus(proc.pid);
                proc.pid = 0;
	        }
        }

        clear();

        // En-tête alignée avec les mêmes largeurs que le tableau
        mvprintw(0, 0,
                 " PID    USER     S    CPU%%    MEM%%   PPID   GID       NAME                          UPTIME");
        mvhline(1, 0, '-', COLS);

        last_pid = draw_process_table(start_pid);

        // Ligne de bas d'écran (rappel des touches principales)
        mvprintw(LINES - 1, 0,
                 "F1/h/?: aide  |  F2/F3: defiler  |  F4: kill  |  F5: pause  |  F6: arret  |  F7: kill  |  F8: redemarer  |  F10/q: quitter");

        refresh();
        usleep(400000); // 0,4 s
    }

    endwin();
}
