////////////////
////////////////////programme incomplet pour le moment 
#include <getopt.h>
#include <sys/sysinfo.h>
#include "main.h"
#include "process.h"
#include "ui.h"
#include "process.h"
#include "network.h"


void help(char* nom){
	printf("Usage %s [Options][argument]\n\n", nom);
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
 		{.name="remote-server",.has_arg=0,.flag=0,.val='s'},
		{.name="username",.has_arg=1,.flag=0,.val='u'},
 		{.name="password",.has_arg=1,.flag=0,.val='p'},
 		{.name="all",.has_arg=1,.flag=0,.val='a'}, // faut utiliser avc -c et -s
		{.name=0,.has_arg=0,.flag=0,.val=0}, // last element must be zero
	};

	//initier des parametres par défaut cette liste va contenir tout les parametres necessaire
	//pour l'utiliser il faut faire param[nom du parametre].parameter_value.type
	//par example pour acceder a la valeure du dry run il faut faire params[DRY_RUN].parameter_value.flag_param
	parameter_t params[] = {
		{.parameter_type=DRY_RUN, .parameter_value.flag_param=false},
                {.parameter_type=REMOTE_CONF, .parameter_value.str_param = ""},
                {.parameter_type=CONNECTION_TYPE, .parameter_value.connection_param=UNDEFINED},//je ne veux pas mettre SSH ou TELNET comme ca je peut distinguer si la personne a mis une valeure ou pas
                {.parameter_type=PORT, .parameter_value.int_param=-1},
                {.parameter_type=LOGIN, .parameter_value.str_param=""},
                {.parameter_type=REMOTE_SERV, .parameter_value.str_param=""},
                {.parameter_type=USERNAME, .parameter_value.str_param=""},
                {.parameter_type=PASSWORD, .parameter_value.str_param=""},
		{.parameter_type=ALL, .parameter_value.flag_param=false}
	};
	int nb_options = 0;
	while((opt = getopt_long(argc, argv, "hdc::t:P:l:s:u:p:a", my_opts, 0)) != -1) {
		switch (opt) {
			case 'h':
				help(argv[0]);
			     return 0;
			case 'd'://dry run
				printf("mode dry run activé.\n");
				params[DRY_RUN].parameter_value.flag_param = true;
				nb_options +=1;
				break;
			case 'c'://remote config
				 if (optind < argc && argv[optind] != NULL && argv[optind][0] != '-'){
				printf("fichier config est %s\n", optarg);
				strncpy(params[REMOTE_CONF].parameter_value.str_param, optarg, STR_MAX - 1);
				nb_options +=1;
				optind++;
				 }
				 else {

					 strncpy(params[REMOTE_CONF].parameter_value.str_param, "defaultConfigFile", STR_MAX - 1);

				 }
				 nb_options += 1;
				break;
			case 't'://connection type
				if (!strcasecmp(optarg,"SSH")) {
					params[CONNECTION_TYPE].parameter_value.connection_param = SSH;
				}
				else if (!strcasecmp(optarg, "TELNET")) {
					params[CONNECTION_TYPE].parameter_value.connection_param = TELNET;
				}
				else{
					printf("erreure dans la saisie de la connection type seulement l'option SSH ou TELNET est autorisée\n");
					help(argv[0]);
				}
				nb_options += 1;
				break;
			case 'P'://port
				if ( atoi(optarg)<0 || atoi(optarg) > 65535){
					printf("argument du port est requis et doit être un entier entre 0 et 65535\n");
					help(argv[0]);//ca quitte automatiquement apres d'afficher l'aide
				}
				params[PORT].parameter_value.int_param = atoi(optarg);
				nb_options += 1;
				break;
			case 'l': //login
				params[LOGIN].parameter_value.int_param = atoi(optarg);
				nb_options += 1;
        			break;
		    case 's': //remote-server
				strncpy(params[REMOTE_SERV].parameter_value.str_param, optarg, STR_MAX - 1);
				nb_options += 1;
			      break;
			case 'u': //username
				strncpy(params[USERNAME].parameter_value.str_param, optarg, STR_MAX - 1);
				nb_options +=1;
				break;
			case 'p': //password
				strncpy(params[PASSWORD].parameter_value.str_param, optarg, STR_MAX - 1);
				nb_options +=1;
				break;
			case 'a': //all
				params[ALL].parameter_value.flag_param = true;
				nb_options += 1;
				break;
		}
	}

	if (params[REMOTE_SERV].parameter_value.str_param != ""){//quand and remote server est spécifié
		//a faire
		//Si les options -u et -p ne sont pas specifiés dans la commande, alors il faudra les demandé interactivement lors de l'exécution.
	}
	run_tui(params);
	return 0;
}
