//¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_//
//_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_// include //¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\\
//¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_//

#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <errno.h>

#define RESET   "\033[0m"
#define YELLOW  "\033[33m"
#define CYAN    "\033[36m"
#define GREEN   "\033[32m"
#define RED     "\033[31m"
#define MAGENTA "\033[35m"

int fenetre_aide(){
    printf(CYAN "htop 3.4.1-3.4.1 - (C) 2004-2019 Hisham Muhammad. (C) 2020-2025 htop dev team.\n" RESET);
    printf(CYAN "Released under the GNU GPLv2+. See 'man' page for more info.\n\n" RESET);

    printf(GREEN "CPU usage bar: " RESET "[low/normal/kernel/guest         used%%]\n");
    printf(GREEN "Memory bar:    " RESET "[used/shared/compressed/buffers/cache         used/total]\n");
    printf(GREEN "Swap bar:      " RESET "[used/cache/frontswap         used/total]\n\n");

    printf(CYAN "Type and layout of header meters are configurable in the setup screen.\n\n" RESET);

    printf(CYAN "Process state: " RESET "R: running; S: sleeping; t: traced/stopped; Z: zombie; D: disk sleep\n\n");
    printf(CYAN "#: " RESET "hide/show header meters\n");
    printf(CYAN "Tab: " RESET "switch to next screen tab\n");
    printf(CYAN "Arrows: " RESET "scroll process list\n");
    printf(CYAN "Digits: " RESET "incremental PID search\n");
    printf(CYAN "F3 /: " RESET "incremental name search\n");
    printf(CYAN "F4 \\\\: " RESET "incremental name filtering\n");
    printf(CYAN "F5 t: " RESET "tree view\n");
    printf(CYAN "p: " RESET "toggle program path\n");
    printf(CYAN "m: " RESET "toggle merged command\n");
    printf(CYAN "Z: " RESET "pause/resume process updates\n");
    printf(CYAN "u: " RESET "show processes of a single user\n");
    printf(CYAN "H: " RESET "hide/show user process threads\n");
    printf(CYAN "K: " RESET "hide/show kernel threads\n");
    printf(CYAN "O: " RESET "hide/show processes in containers\n");
    printf(CYAN "F: " RESET "cursor follows process\n");
    printf(CYAN "+ - *: " RESET "expand/collapse tree/toggle all\n");
    printf(CYAN "N P M T: " RESET "sort by PID, CPU%%, MEM%% or TIME\n");
    printf(CYAN "I: " RESET "invert sort order\n");
    printf(CYAN "F6 > .: " RESET "select sort column\n");
    printf(CYAN "S-Tab: " RESET "switch to previous screen tab\n");
    printf(CYAN "Space: " RESET "tag process\n");
    printf(CYAN "c: " RESET "tag process and its children\n");
    printf(CYAN "U: " RESET "untag all processes\n");
    printf(RED "F9 k: " RESET "kill process/tagged processes\n");
    printf(CYAN "F7 ]: " RESET "higher priority (root only)\n");
    printf(CYAN "F8 [: " RESET "lower priority (+ nice)\n");
    printf(CYAN "a: " RESET "set CPU affinity\n");
    printf(CYAN "e: " RESET "show process environment\n");
    printf(CYAN "i: " RESET "set IO priority\n");
    printf(CYAN "l: " RESET "list open files with lsof\n");
    printf(CYAN "x: " RESET "list file locks of process\n");
    printf(CYAN "s: " RESET "trace syscalls with strace\n");
    printf(CYAN "w: " RESET "wrap process command in multiple lines\n");
    printf(CYAN "y: " RESET "set scheduling policy\n\n");
    printf(MAGENTA "F2 C S: " RESET "setup\n");
    printf(MAGENTA "F1 h ?: " RESET "show this help screen\n");
    printf(MAGENTA "F10 q: " RESET "quit\n\n\n\n");
    printf(YELLOW "Press any key to return.\n" RESET);
    return 0;
}

//teste de pros
typedef struct{
    int pid;
    char name[64];
    int nice;
} Process;

Process processes[] = {
    {1234, "bash", 0},
    {2345, "htop", 5},
    {3456, "vim", 10},
    {4567, "gcc", -5}
};
int process_count = 4;



//¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_//
//_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_// complement fonc //¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\\
//¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_//

//////////////////////////F6
// Fonction de tri par PID
int cmp_pid(const void *a, const void *b){
    return ((Process*)a)->pid - ((Process*)b)->pid;
}

// Fonction de tri par nom
int cmp_name(const void *a, const void *b){
    return strcmp(((Process*)a)->name, ((Process*)b)->name);
}

// Fonction de tri par nice
int cmp_nice(const void *a, const void *b){
    return ((Process*)a)->nice - ((Process*)b)->nice;
}



//////////////////////////F9
volatile pid_t target_pid = -1; // PID du processus à tuer // demander le pid avant

void selectProcess(void){

    printf("=== Liste des processus actifs ===\n");             //
    // Affiche PID + nom du programme                           //a retirer
    system("ps -e -o pid,comm");                                //

    printf("\nEntrez le PID du processus à cibler: ");
    scanf("%d", &target_pid);
    printf(">> PID %d sélectionné\n", target_pid);
}

//////////////////////////F10
volatile int running = 1; // variable globale pour contrôler l'exécution




//¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_//
//_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_//  pseudo //¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\\
//¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_//


////////////////////////////////////F1            Help                                                                      ouvre / ferme la fenetre d'aide
//si F1 appuyee alors
    //ouvrir_fenetre_aide()
    //attendre_touche()
    //fermer_fenetre_aide()
//fin si


////////////////////////////////////F2            Setup


////////////////////////////////////F3            Search


////////////////////////////////////F4            Filter


////////////////////////////////////F5            Tree


////////////////////////////////////F6            SortBy


////////////////////////////////////F7            Nice -


////////////////////////////////////F8            Nice +


////////////////////////////////////F9            Kill


////////////////////////////////////F10           Quit                                                                             Arret complet du programe





//¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_//
//_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_//   F..   //¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\\
//¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_//


/////////////////////////////////////F1             Help                                                                                            Complete
//fait appele a la fenetre d aide
void F1_help(){

    fenetre_aide();                                                                          //ouvre une fenetre d aide

    getchar();// attend une touche
}



////////////////////////////////////F2              Setup



////////////////////////////////////F3              Search
void F3_search(){
    char query[128];
    printf("\n=== RECHERCHE (F3) ===\n");
    printf("Tapez votre texte puis Entrée: ");
    fflush(stdout);

    // Remet temporairement le terminal en mode canonique pour lire une ligne
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag |= ICANON | ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    if (fgets(query, sizeof(query), stdin) != NULL){
        // Supprimer le \n
        query[strcspn(query, "\n")] = 0;
        printf("Vous avez recherché: %s\n", query);
    }

    // Revenir au mode non canonique
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}



////////////////////////////////////F4              Filter
void F4_filter(){
    char query[128];
    printf("\n=== FILTRE (F4) ===\n");
    printf("Entrez un texte pour filtrer puis appuyez sur Entrée: ");
    fflush(stdout);

    // Remet temporairement le terminal en mode canonique pour lire une ligne
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag |= ICANON | ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    if (fgets(query, sizeof(query), stdin) != NULL){
        query[strcspn(query, "\n")] = 0; // supprime le \n
        printf("Filtrage sur: %s\n", query);

        // Exemple de données à filtrer
        const char* items[] = {"processus1", "apache", "mysql", "htop", "processus2"};
        int count = sizeof(items)/sizeof(items[0]);

        printf("Résultats:\n");
        for (int i = 0; i < count; i++){
            if (strstr(items[i], query)){
                printf(" - %s\n", items[i]);
            }
        }
    }

    // Retour au mode non canonique
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}



////////////////////////////////////F5              Tree
void F5_tree(void){
    printf("\n=== Arbre des processus ===\n");
    // Appel direct à pstree (si installé sur le système)
    int ret = system("pstree -p");
    if (ret != 0){
        printf(">> Impossible d’exécuter pstree (non installé ?)\n");
    }
}



////////////////////////////////////F6              SortBy
void F6_sort_by(void){
    int choix;
    printf("\n=== Choisir le tri ===\n");
    printf("1. PID\n");
    printf("2. Nom\n");
    printf("3. Nice\n");
    printf("Votre choix: ");
    scanf("%d", &choix);

    switch (choix){
        case 1:
            qsort(processes, process_count, sizeof(Process), cmp_pid);
            printf(">> Tri par PID effectué\n");
            break;
        case 2:
            qsort(processes, process_count, sizeof(Process), cmp_name);
            printf(">> Tri par Nom effectué\n");
            break;
        case 3:
            qsort(processes, process_count, sizeof(Process), cmp_nice);
            printf(">> Tri par Nice effectué\n");
            break;
        default:
            printf(">> Choix invalide\n");
            return;
    }

    // Afficher la liste triée
    printf("\n=== Liste triée ===\n");                                                //
    for (int i = 0; i < process_count; i++) {                                         //
        printf("PID: %d | Nom: %s | Nice: %d\n",                                      //mettre un affichage trie de la liste de pros
               processes[i].pid, processes[i].name, processes[i].nice);               //
    }
}



////////////////////////////////////F7              Nice -
void F7_nice_plus(void){
    if (target_pid > 0){
        // Récupérer la valeur actuelle
        int current = getpriority(PRIO_PROCESS, target_pid);
        if (current == -1 && errno != 0){
            perror("getpriority");
            return;
        }
        int newval = current + 1;
        if (setpriority(PRIO_PROCESS, target_pid, newval) == 0){
            printf(">> Nice augmenté: PID %d, valeur %d\n", target_pid, newval);
        } else{
            perror("setpriority");
        }
    } else{
        printf(">> Aucun processus sélectionné\n");
    }
}



////////////////////////////////////F8              Nice +
void F8_nice_minus(void){
    if (target_pid > 0){
        int current = getpriority(PRIO_PROCESS, target_pid);
        if (current == -1 && errno != 0){
            perror("getpriority");
            return;
        }
        int newval = current - 1;
        if (setpriority(PRIO_PROCESS, target_pid, newval) == 0){
            printf(">> Nice diminué: PID %d, valeur %d\n", target_pid, newval);
        } else{
            perror("setpriority");
        }
    } else{
        printf(">> Aucun processus sélectionné\n");
    }
}



////////////////////////////////////F9              Kill
void F9_kill(void){
    if (target_pid > 0){
        if (kill(target_pid, SIGKILL) == 0){
            printf(">> Processus %d tué avec succès\n", target_pid);
        } 
        else{
            perror(">> Erreur lors du kill");
        }
    } 
    else{
        printf(">> Aucun processus sélectionné\n");
    }
}



////////////////////////////////////F10             Quit
//fai dan le prog principal








//¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_//
//_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_//  teste  //¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\\
//¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_//


///////////////////////////////////gere l appui de nimporte quelle touche




void initKeyboard(){
    struct termios t;
    tcgetattr(STDIN_FILENO, &t);
    t.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
}

/*
int detectKeyPress(){
    char buf[8];
    int n = read(STDIN_FILENO, buf, sizeof(buf));
    if (n > 0){
        buf[n] = '\0';
        // Retourne la séquence complète
        return n; 
    }
    return 0;
}*/


void* keyboardThread(void* arg){
    char buf[8];
    while (running){
        int n = read(STDIN_FILENO, buf, sizeof(buf)-1);
        if (n > 0){
            buf[n] = '\0';

            // Vérification des séquences F1-F10
            //F1
            if (strcmp(buf, "\033OP") == 0){                    ///
                printf("F1 appuyée\n"); 
                F1_help();
            }

            //F2  
            else if (strcmp(buf, "\033OQ") == 0){               
                printf("F2 appuyée\n");   
            }

            //F3
            else if (strcmp(buf, "\033OR") == 0){               ///
                printf("F3 appuyée\n");
                F3_search();
            }

            //F4   
            else if (strcmp(buf, "\033OS") == 0){               ///
                printf("F4 appuyée\n");
                F4_filter();
            }

            //F5
            else if (strcmp(buf, "\033[15~") == 0){
                printf("F5 appuyée\n");
                F5_tree();
            }

            //F6
            else if (strcmp(buf, "\033[17~") == 0){
                printf("F6 appuyée\n");
                F6_sort_by();
            }

            //F7
            else if (strcmp(buf, "\033[18~") == 0){ 
                printf("F7 appuyée\n");
                selectProcess();
                F7_nice_plus();
            }
             
            //F8
            else if (strcmp(buf, "\033[19~") == 0){
                printf("F8 appuyée\n");
                selectProcess();
                F8_nice_minus();
            }

            //F9
            else if (strcmp(buf, "\033[20~") == 0){             ///
                printf("F9 appuyée\n");
                selectProcess();
                F9_kill();
            }
            else if (strcmp(buf, "\033[21~") == 0){             ///
                printf("F10 appuyée\n");
                running = 0; // stoppe la boucle
                break;
            }
                
            else if (buf[0] == 27 && n == 1) { // ESC simple
                printf("ESC détectée, sortie...\n");
                running = 0; // stoppe la boucle
                break;
            }
        }
        usleep(10000);
    }
    return NULL;
}

int main() {
    initKeyboard();
    pthread_t tid;
    pthread_create(&tid, NULL, keyboardThread, NULL);

    // Code principal en parallèle
    for (int i = 0; i < 10 && running; i++) {
        printf("Travail principal: %d\n", i);
        sleep(1);
    }

    pthread_join(tid, NULL);
    printf("Programme terminé.\n");
    return 0;
}5
