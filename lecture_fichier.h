#include <stdbool.h>
typedef struct serveurs{
	char *nom_serveur;
	char addr[15];
	unsigned short port;
	char *utilisateur;
	char *mdp;
	char type[7];
	struct serveurs *next;
}serveurs;

#define BUFFER 1024

serveurs *lirefichier(const char *chemin);//lis le contenu du fichier remote_conf.txt et retourne les serveurs dans une structure
void free_serveurs (serveurs *tete); // il faut toujours appeler cette fonction aprés de finir d'utilser la fonction pour liberer la memoire.

serveurs *creer_serveur(char *ligne);
bool verif_permissions(const char *chemin); // retourve vrai si les permissions sont rw-------
const char *chemin_conf(); // cherche le chemin de config dan3s le repertoire courant

serveurs *suivant(serveurs *serv);
