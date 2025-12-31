#include "lecture_fichier.h"
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>

const char *chemin_conf(){
	char rep_courant[BUFFER];//buffer pour avoir le repertoire courant
				 
	if (getcwd(rep_courant, sizeof(rep_courant)) == NULL) {
        	printf("erreure : getcwd\n");
		exit(EXIT_FAILURE);
	}


	struct dirent *contenu_dossier;
	DIR *dp = opendir(rep_courant);

	if (dp == NULL){
		printf("erreur : opendir");
		exit(EXIT_FAILURE);
	}
	while ((contenu_dossier = readdir(dp)) != NULL){
		//voir si le nom finis par .conf
		int len = strlen(contenu_dossier->d_name);
		char *nom = contenu_dossier->d_name;
		if(len >= 5 && strcmp( nom+len-5, ".conf") == 0){
			char *chemin_complet = malloc(strlen(rep_courant) + len + 2);
			
			if (chemin_complet == NULL){
				printf("erreur : malloc\n");
				closedir(dp);
				exit(EXIT_FAILURE);
			}
			sprintf(chemin_complet, "%s/%s", rep_courant, nom);
			closedir(dp);
			return chemin_complet;// retourner le chemin complét apres de fermer le dossier
		}

	}

	closedir(dp);
	return NULL; // fichier non trouvé
}


bool verif_permissions(const char *chemin){// retourve vrai si les permissions sont rw-------
	struct stat stat_f;

	//obtenir les stats du fichier
	if (stat(chemin, &stat_f) == -1){
		printf("erreure de lecture des stats\n");
		exit(EXIT_FAILURE);
	}

	bool bonne_permission;
	bonne_permission = (stat_f.st_mode & S_IRUSR) &&  (stat_f.st_mode & S_IWUSR);// on verifie si l'utilisateur a le droit d'écriture et de lecture. 

	bonne_permission = bonne_permission && 
		!(stat_f.st_mode & S_IXUSR) &&//pas de permission d'execution pour l'utilisateur.
		//pour le groupe il doit avoire auqune permission
		 !(stat_f.st_mode & S_IRGRP) &&
		 !(stat_f.st_mode & S_IWGRP) &&
		 !(stat_f.st_mode & S_IXGRP) &&
		 //et pour l'autre il doit avoir aucune permission non plus
		 !(stat_f.st_mode & S_IROTH) &&
		 !(stat_f.st_mode & S_IWOTH) &&
		 !(stat_f.st_mode & S_IXOTH);
	return bonne_permission;
}

serveurs *creer_serveur(char *ligne){
	serveurs *nouvel_serveur = malloc(sizeof(serveurs));
	if (nouvel_serveur == NULL){
		printf("erreur : malloc\n");
		exit(EXIT_FAILURE);
	}

	char *token;

	token = strtok(ligne, ":");
	nouvel_serveur->nom_serveur = strdup(token);//on duplique le nom pour ne pas avoir d'erreures
	
	token = strtok(NULL, ":");
	strncpy(nouvel_serveur->addr, token, sizeof(nouvel_serveur->addr));
	nouvel_serveur->addr[sizeof(nouvel_serveur->addr) -1] = '\0';

	token = strtok(NULL, ":");
	nouvel_serveur->port = (unsigned short)atoi(token);

	token = strtok(NULL, ":");
	nouvel_serveur->utilisateur = strdup(token);

	token = strtok(NULL, ":");
	nouvel_serveur->mdp = strdup(token);

	token = strtok(NULL, ":");
	strncpy(nouvel_serveur->type, token, sizeof(nouvel_serveur->type) - 1);

	nouvel_serveur->next = NULL;
	return nouvel_serveur;


}

void  free_serveurs( serveurs *tete){
	serveurs *serveur = tete;
	while (tete != NULL){
		serveurs *next = serveur->next;
		free(serveur->nom_serveur);
		free(serveur->utilisateur);
		free(serveur->mdp);
		free(serveur);
		serveur = next;
	}

}
serveurs *lirefichier(const char *chemin){
	FILE *file;
	char ligne[BUFFER];

	if (chemin == NULL){// chemin non fourni il faut le chercher dans le repertoire courant

		if((chemin = chemin_conf()) == NULL){//pas de .conf trouvé
			printf("pas de .conf fourni ni trouvé dans le repertoire courant. Utiliser le drapeau -c ou --remote-config pour spécifier un chemin");
			exit(EXIT_FAILURE);
		}
		else {
			printf("Chemin de config trouvé : %s\n", chemin);
		}

	}

	if (!verif_permissions(chemin)){
		printf("\033[0;31m AVERTISSEMENT: fichier n'a pas les bons droits il faut avoir les droits suivants rw------- ou 600\033[0;31m");
	
	}
	file= fopen(chemin, "r");

	if (file == NULL){
		printf("Erreure d'ouverture du fichier\n");
		exit(EXIT_FAILURE);
	}
	

	serveurs *tete = NULL;	
	serveurs *queue = NULL;

	//on commence a lire le fichier.
	while (fgets(ligne, sizeof(ligne), file)){
		ligne[strcspn(ligne, "\n")] = '\0';//enlever la nouvelle ligne

		serveurs *nouvel_serveur = creer_serveur(ligne);

		if (tete == NULL){
			tete = nouvel_serveur;
		}
		else {
			queue->next = nouvel_serveur;
		}
		queue = nouvel_serveur;
	}

	fclose(file);
	return tete;
}


