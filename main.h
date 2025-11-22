#ifndef MAIN_H
#define MAIN_H

#define STR_MAX 1024 // le maximum de charactéres qu'on autorise pour un str
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
typedef enum {
	DRY_RUN,
	REMOTE_CONF,
	CONNECTION_TYPE,
	PORT,
	LOGIN,
	REMOTE_SERV,
	USERNAME,
	PASSWORD,
	ALL
} parameter_id_t;

typedef enum {
	SSH,
	TELNET,
	UNDEFINED
}type_connection_t;
typedef union {
	long long int_param; //pour le port
	char str_param[STR_MAX];// pour le username, password, login et 
	bool flag_param; //pour le dryrun
	type_connection_t connection_param; //pour le type de connection
} data_wrapper_t;

typedef struct {
	parameter_id_t parameter_type;
	data_wrapper_t parameter_value;
}parameter_t;

#endif







