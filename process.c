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

void info_processus(pid_t pid) {
    char path[256], buffer[256];
    FILE *file;

    printf("\n--- Informations sur le processus %d ---\n", pid);

    // --------- IDENTITÉ UTILISATEUR ---------
    struct stat st;
    snprintf(path, sizeof(path), "/proc/%d", pid);

    if (stat(path, &st) == -1) {
        perror("PID introuvable");
        return;
    }

    struct passwd *pw = getpwuid(st.st_uid);
    printf("Utilisateur : %s\n", pw ? pw->pw_name : "Inconnu");

    // --------- NOM DU PROCESSUS ---------
    snprintf(path, sizeof(path), "/proc/%d/comm", pid);
    if ((file = fopen(path, "r"))) {
        fgets(buffer, sizeof(buffer), file);
        printf("Nom : %s", buffer);
        fclose(file);
    }

    // --------- MÉMOIRE ---------
    snprintf(path, sizeof(path), "/proc/%d/status", pid);
    if ((file = fopen(path, "r"))) {
        while (fgets(buffer, sizeof(buffer), file)) {
            if (strncmp(buffer, "VmRSS:", 6) == 0) {
                printf("Mémoire utilisée : %s", buffer + 8);
            }
        }
        fclose(file);
    }

    // --------- CPU + TEMPS D'EXÉCUTION ---------

    long utime, stime, starttime;
    long clock_ticks = sysconf(_SC_CLK_TCK);
    long uptime = 0;

    FILE *uptime_file = fopen("/proc/uptime", "r");
    if (uptime_file) {
        fscanf(uptime_file, "%ld", &uptime);
        fclose(uptime_file);
    }

    snprintf(path, sizeof(path), "/proc/%d/stat", pid);
    file = fopen(path, "r");

    if (file) {
        int pid_stat;
        char comm[64], state;

        fscanf(file,
               "%d %s %c %*d %*d %*d %*d %*d %*u %*u %*u %*u %ld %ld %*d %*d %*d %*d %*d %ld",
               &pid_stat, comm, &state,
               &utime, &stime, &starttime);

        fclose(file);

        double total_time = (utime + stime) / (double)clock_ticks;
        double seconds = uptime - (starttime / clock_ticks);

        printf("Temps CPU : %.2f sec\n", total_time);
        printf("Temps d'exécution : %.2f sec\n", seconds);
    }
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
