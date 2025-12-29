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
#include "gestion_fenetre.h"
#include "ui.h"
#include "process.h"


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
                show_help_window(void) 
            }

            //F2
            else if (strcmp(buf, "\033OQ") == 0){
                printf("F2 appuyée\n");
                F2_setup();                                                       /// fonction onglet suivant 
            }

            //F3
            else if (strcmp(buf, "\033OR") == 0){               ///
                printf("F3 appuyée\n");
                F3_search();                                                       /// fonction onglet precedent
            }

            //F4     recherche pros
            else if (strcmp(buf, "\033OS") == 0){               ///
                printf("F4 appuyée\n");
                rechercher_processus(const char *nom);       ////demande le char avant 
            } 
            //F5
            else if (strcmp(buf, "\033[15~") == 0){
                printf("F5 appuyée\n");
                pause_processus(pid_t pid);                   ////met en pose mais deamande avant
            }

            //F6
            else if (strcmp(buf, "\033[17~") == 0){
                printf("F6 appuyée\n");
                arret_processus(pid_t pid);
            }

            //F7
            else if (strcmp(buf, "\033[18~") == 0){ 
                printf("F7 appuyée\n");
                selectProcess();
                F7_nice_plus();                                      ///tuer proce a changer
            }

            //F8
            else if (strcmp(buf, "\033[19~") == 0){
                printf("F8 appuyée\n");
                selectProcess();
                redemarrer_processus(pid_t pid);
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
}
