//¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_//
//_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_// include //¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\\
//¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_//


#include <stdio.h>
#include <unistd.h>
#include <conio.h>

#define RESET   "\033[0m"
#define YELLOW  "\033[33m"
#define CYAN    "\033[36m"
#define GREEN   "\033[32m"
#define RED     "\033[31m"
#define MAGENTA "\033[35m"



int fenetre_aide() {
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





//¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_//
//_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_// complement fonc //¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\\
//¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_//

///////////////////////////////complement F1
int attendre_touche(){
    printf("Appuie sur une touche pour continuer...\n");
    getch(); // Attendre l'appui d'une touche
    return 0;
}

///////////////////////////////complement F10
void stop_program() {
    printf("Arrêt du programme...\n");
    exit(0);
}





//¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_//
//_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_//  pseudo //¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\\
//¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_//


////////////////////////////////////F1            Help                                                      ouvre / ferme la fenetre d'aide
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


////////////////////////////////////F10           Quit                                                                                   Arret complet du programe





//¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_//
//_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_//   F..   //¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\\
//¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_//


/////////////////////////////////////F1             Help                                                                                                   Complete
//fait appele a la fenetre d aide
void F1_help() {
    fenetre_aide();
    attendre_touche();                  // est utiliser pour bloquer la fenetre d aide
    // fermer_fenetre_aide()    ou     repartir sur le menu principal
}



////////////////////////////////////F2              Setup



////////////////////////////////////F3              Search
void F3_search() {
    int i = 1;

    while (i == 1) {
        int ch = getch();
        if (ch == 0 || ch == 224) {
            // touche spéciale
            int special = getch();
            switch (special) {
                case 61: printf("F3\n"); break;

                case 72: printf("Flèche haut\n"); break;
                case 80: printf("Flèche bas\n"); break;
                case 75: printf("Flèche gauche\n"); break;
                case 77: printf("Flèche droite\n"); break;

                default: printf("Touche spéciale code: %d\n", special);
            }
        } 
        if(ch == 27) {
            // touche simple
            printf("Echap\n");
        }
    }
    // implémentation de la fonction de recherche
}



////////////////////////////////////F4              Filter


////////////////////////////////////F5              Tree


////////////////////////////////////F6              SortBy


////////////////////////////////////F7              Nice -


////////////////////////////////////F8              Nice +


////////////////////////////////////F9              Kill



////////////////////////////////////F10             Quit
void F10_quit() {
    stop_program();
}










//¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_//
//_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_//  teste  //¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\\
//¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_/¯\_//


///////////////////////////////////gere l appui de nimporte quelle touche
int appele_touche(){        
    
    printf("Appuie sur une touche...\n");

    int ch = getch();
    printf("Code de la touche appuyée: %d\n", ch);
    if (ch == 0 || ch == 224) {
        // touche spéciale
        int special = getch();
        switch (special) {
            case 27: printf("Echap\n"); break;

            case 59: printf("F1\n"); F1_help(); break;
            case 60: printf("F2\n"); break;
            case 61: printf("F3\n"); /*F3_search();*/ break;
            case 62: printf("F4\n"); break;
            case 63: printf("F5\n"); break;
            case 64: printf("F6\n"); break;
            case 65: printf("F7\n"); break;
            case 66: printf("F8\n"); break;
            case 67: printf("F9\n"); break;
            case 68: printf("F10\n"); break;

            case 72: printf("Flèche haut\n"); break;
            case 80: printf("Flèche bas\n"); break;
            case 75: printf("Flèche gauche\n"); break;
            case 77: printf("Flèche droite\n"); break;

            default: printf("Touche spéciale code: %d\n", special);
        }
    } else {
        // touche simple
        printf("Touche simple: '%c' (code %d)\n", ch, ch);
    }

    return 0;
}





///////////////////////tete pour 2 touche 
int doublej() {
    printf("Appuie sur une combinaison de 2 touches...\n");

    int ch1 = getch();   // première touche
    int ch2 = getch();   // deuxième touche

    if (ch1 == 27 && ch2 == 0) { // ESC puis touche spéciale
        int special = getch();   // lecture du code spécial
        if (special == 59) {
            printf("Combinaison ESC + F1 détectée\n");
        }
    } else if (ch1 == 0 || ch1 == 224) {
        // première touche spéciale
        int special1 = getch();
        if (special1 == 68 && ch2 == 27) {
            printf("Combinaison F10 + ESC détectée\n");
        }
    } else {
        printf("Touche 1: %d, Touche 2: %d\n", ch1, ch2);
    }
    printf("Fin de la détection des combinaisons de touches.\n");
    return 0;
}






int main() {
    doublej();
    return 0;
}
