#include <getopt.h>
#include <stdio.h>
#include <sys/sysinfo.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

void help(char* nom){
	printf("Usage %s [Options]\n\n", nom);
	printf("--help ou -h		afficher ce message et sortir\n\n");
	printf("--dry-run		test l'accès à la liste des processus sur la machine locale et/ou distante sans les afficher\n\n");
	printf("--remote-config ou -c	spécifie le chemin vers le fichier de configuration contenant les informations de connexion\n			sur les machines distantes\n\n");
	printf("--connexiton-type ou -t spécifie le type de connexion à utiliser pour la connexion sur les machines distantes (ssh, telnet)\n\n");
	printf("--port ou -P 		spécifie le port à utiliser pour le type de connexion choisi. Si cette option n'est pas spécifié,\n			alor le port par défaut du type de connexion est choisi\n\n");
	printf("--login ou -l		spécifie l'identifiant de connexion et la machine distante. Ex : --login user@remote_server.\n			remote_server est soit l'IP ou le nom DNS de la machine distante\n\n");
	printf("--remote-server ou -s	spécifie le nom DNS ou l'IP de la machine distante\n\n");
	printf("--username ou -u	spécifie le nom de l'utilisateur à utiliser pour la connexion\n\n");
	printf("--password ou -p	spécifie le mot de passe à utiliser pour la connexion\n\n");
	printf("--all ou -a		spécifie au programme de collecter à la fois la liste des processus sur la machine local et les machines distantes.\n		S'utilise uniquement si l'option -c ou -s est utilisé.\n\n");
}
int main(int argc, char *argv[]) {
	int opt = 0;
	struct option my_opts[] = {
		{.name="help",.has_arg=0,.flag=0,.val='h'},
		{.name="dry-run",.has_arg=0,.flag=0,.val='d'},
		{.name="remote-config",.has_arg=1,.flag=0,.val='c'},
		{.name="connection-type",.has_arg=1,.flag=0,.val='t'},
		{.name="port",.has_arg=1,.flag=0,.val='P'},
 		{.name="login",.has_arg=1,.flag=0,.val='l'},
 		{.name="remote-server",.has_arg=1,.flag=0,.val='s'},
    {.name="username",.has_arg=1,.flag=0,.val='u'},
 		{.name="password",.has_arg=1,.flag=0,.val='p'},
 		{.name="all",.has_arg=1,.flag=0,.val='a'}, // faut utiliser avc -c et -s
		{.name=0,.has_arg=0,.flag=0,.val=0}, // last element must be zero
	};
	char* source = 0;
	char* temp_directory = 0;
	char* outf = 0;
	bool verbose = 0;
	while((opt = getopt_long(argc, argv, "h", my_opts, 0)) != -1) {
		switch (opt) {
			case 'h':
				help(argv[0]);
			       return 0;
			case 'd'://dry run
				break;
			case 'c'://remote config
				break;
			case 't'://connection type
				break;
			case 'P'://port
				break;
      case 'l': //login
        break;
      case 's': //remote-server
        break;
      case 'u': //username
        break;
      case 'p': //password
        break;
      case 'a': //all
        break;
		}
	}
	return 0;
}
