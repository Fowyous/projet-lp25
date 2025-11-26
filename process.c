#include "process.h"

// ---------------------------------------------------------------
// 1. AFFICHER LA LISTE DYNAMIQUE DES PROCESSUS
// ---------------------------------------------------------------

void afficher_processus() {
    DIR *dir = opendir("/proc");
    struct dirent *entry;

    if (!dir) {
        perror("Erreur ouverture /proc");
        return;
    }

    printf("Liste des processus actifs :\n");

    while ((entry = readdir(dir)) != NULL) {
        if (isdigit(entry->d_name[0])) {
            printf("PID : %s\n", entry->d_name);
        }
    }

    closedir(dir);
}

// ---------------------------------------------------------------
// 2. AFFICHER LES INFORMATIONS D’UN PROCESSUS
// ---------------------------------------------------------------

proc_info_t get_process_info(pid_t pid) {
    proc_info_t info;
    memset(&info, 0, sizeof(info));
    info.pid = pid;

    char path[256], buffer[256];
    FILE *file;

    //-------------------------------------
    // 1. USER / UID / GID
    //-------------------------------------
    struct stat st;
    snprintf(path, sizeof(path), "/proc/%d", pid);

    if (stat(path, &st) == -1) {
        perror("PID introuvable");
        info.pid = -1;
        return info;
    }

    struct passwd *pw = getpwuid(st.st_uid);
    snprintf(info.user, sizeof(info.user), "%s", pw ? pw->pw_name : "Inconnu");
    info.gid = st.st_gid;

    //-------------------------------------
    // 2. Nom du processus
    //-------------------------------------
    snprintf(path, sizeof(path), "/proc/%d/comm", pid);
    if ((file = fopen(path, "r"))) {
        fgets(info.name, sizeof(info.name), file);
        info.name[strcspn(info.name, "\n")] = 0;  // enlever le \n
        fclose(file);
    }

    //-------------------------------------
    // 3. Lecture du stat pour CPU, état, PPID, etc.
    //-------------------------------------

    long utime, stime, starttime;
    long vsize = 0;
    long rss = 0;
    long clock_ticks = sysconf(_SC_CLK_TCK);

    snprintf(path, sizeof(path), "/proc/%d/stat", pid);
    file = fopen(path, "r");

    if (file) {
        /*
            Format simplifié :
            1 = pid
            2 = comm (entre parenthèses)
            3 = state
            4 = ppid
            ...
            14 = utime
            15 = stime
            22 = starttime
            24 = vsize
            25 = rss
        */
        int dummy_pid, ppid;
        char comm[128], state;

        fscanf(file,
            "%d %s %c %d "
            "%*d %*d %*d %*d %*u %*u %*u %*u "
            "%ld %ld %*d %*d %*d %*d %*d %ld %ld %ld",
            &dummy_pid, comm, &state, &ppid,
            &utime, &stime, &starttime, &vsize, &rss
        );

        fclose(file);

        info.state = state;
        info.ppid = ppid;

        // CPU %
        double total_time = (utime + stime) / (double)clock_ticks;

        double uptime_system = 0;
        FILE *upt = fopen("/proc/uptime", "r");
        if (upt) {
            fscanf(upt, "%lf", &uptime_system);
            fclose(upt);
        }

        double seconds = uptime_system - (starttime / clock_ticks);
        info.uptime_seconds = seconds;

        if (seconds > 0)
            info.cpu_percent = 100.0 * (total_time / seconds);
    }

    //-------------------------------------
    // 4. Mémoire % (VmRSS)
    //-------------------------------------
    long mem_total = 0;
    file = fopen("/proc/meminfo", "r");
    if (file) {
        while (fgets(buffer, sizeof(buffer), file)) {
            if (sscanf(buffer, "MemTotal: %ld kB", &mem_total) == 1)
                break;
        }
        fclose(file);
    }

    if (mem_total > 0)
        info.mem_percent = 100.0 * ((rss * 4) / (double)mem_total); // rss*4 = kB

    return info;
}

// ---------------------------------------------------------------
// 3. INTERACTION AVEC LES PROCESSUS
// ---------------------------------------------------------------

void pause_processus(pid_t pid) {
    if (kill(pid, SIGSTOP) == 0)
        printf("Processus %d mis en pause\n", pid);
    else
        perror("Erreur SIGSTOP");
}

void reprise_processus(pid_t pid) {
    if (kill(pid, SIGCONT) == 0)
        printf("Processus %d repris\n", pid);
    else
        perror("Erreur SIGCONT");
}

void arret_processus(pid_t pid) {
    if (kill(pid, SIGTERM) == 0)
        printf("Processus %d arrêté\n", pid);
    else
        perror("Erreur SIGTERM");
}

void redemarrer_processus(pid_t pid) {
    char path[256];
    snprintf(path, sizeof(path), "/proc/%d/exe", pid);

    char exe_path[256];
    ssize_t len = readlink(path, exe_path, sizeof(exe_path) - 1);

    if (len == -1) {
        perror("Impossible de lire /proc/<pid>/exe");
        return;
    }

    exe_path[len] = '\0';

    printf("Redémarrage du processus %d (%s)\n", pid, exe_path);

    kill(pid, SIGTERM);
    sleep(1);

    if (fork() == 0) {
        execl(exe_path, exe_path, NULL);
        perror("Échec relance");
    }
}

// ---------------------------------------------------------------
// 4. MENU PRINCIPAL
// ---------------------------------------------------------------

// int main() {
//     int choix;
//     pid_t pid;

//     do {
//         printf("\n===== GESTIONNAIRE DE PROCESSUS =====\n");
//         printf("1. Afficher tous les processus\n");
//         printf("2. Infos sur un processus\n");
//         printf("3. Mettre en pause un processus\n");
//         printf("4. Reprendre un processus\n");
//         printf("5. Arrêter un processus\n");
//         printf("6. Redémarrer un processus\n");
//         printf("0. Quitter\n");
//         printf("Choix : ");
//         scanf("%d", &choix);

//         if (choix != 1 && choix != 0) {
//             printf("Entrez le PID : ");
//             scanf("%d", &pid);
//         }

//         switch (choix) {
//             case 1: afficher_processus(); break;
//             case 2: info_processus(pid); break;
//             case 3: pause_processus(pid); break;
//             case 4: reprise_processus(pid); break;
//             case 5: arret_processus(pid); break;
//             case 6: redemarrer_processus(pid); break;
//         }

//     } while (choix != 0);

//     return 0;
// }
