#include <getopt.h>
#include <stdio.h>
#include <sys/sysinfo.h>
#include <stdlib.h>
#include <stdbool.h>

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
	while((opt = getopt_long(argc, argv, "", my_opts, 0)) != -1) {
		switch (opt) {
			case 'h':
				printf("option verbose\n");
        break;
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
