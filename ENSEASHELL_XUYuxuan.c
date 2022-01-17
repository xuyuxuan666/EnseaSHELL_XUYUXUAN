#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>  
#include <time.h>
#include <sys/wait.h>


//Q1 Affichage d’un message
#define accueil "Bienvenue dans le Shell ENSEA. \nPour quitter, tapez 'exit'.\nenseash % ~ > "
//
#define base  "enseash % ~ >"
//Q3
# define bye "Bye bye...\n"

char **decoupe(char *saisie){
	char** cmd=NULL;  // resultat  Q6
	int retirer=0;
	int num_argument=0;
	const char s[1] = " ";
	char *token;
	token = strtok(saisie, s);
	while( token != NULL ) {
		retirer=strlen(token);
		num_argument++;	
		cmd=(char**) realloc(cmd,sizeof(char*)*(num_argument));
		cmd[num_argument-1]=(char*) malloc( sizeof(char)*(retirer+1) );
		strncpy(cmd[num_argument-1],token,retirer);
		cmd[num_argument-1][retirer]='\0';     
  		token = strtok(NULL, s);
	}	
	num_argument++;
    cmd= (char**) realloc(cmd,sizeof(char*)*num_argument);
    cmd[num_argument-1]=NULL;	  
	return cmd;	
}


int main() {
	

	write(STDOUT_FILENO, accueil, sizeof(accueil));
		
	while(1){
		char saisie[512]=""; 
		struct timespec start, end;
		int nread=read(STDIN_FILENO,saisie,sizeof(saisie));
		saisie[strlen(saisie)-1]='\0';		
		char** cmd = decoupe(saisie);		
		
		if(strncmp("exit",saisie,5)==0|| nread==0){
			write(STDOUT_FILENO, bye, sizeof(bye));			
			exit(EXIT_SUCCESS);
		}	
		
		int pid , status;
		pid=fork();
	
		if(pid== 0){  ///on est dans le processus du fil
			execvp(cmd[0],cmd);
			exit(EXIT_SUCCESS);
		}else if(pid>0){ ///on est dans le processus du père
			clock_gettime( CLOCK_MONOTONIC, &start ); //la fonction « clock_gettime() »
			wait(&status);
		}else{
			write(STDOUT_FILENO, "error fork", sizeof("error fork"));
		}
			clock_gettime( CLOCK_MONOTONIC, &end ); //"date"
		
		char buf[512]="";
		if(WIFEXITED(status)){ //Q5 On calculera le delta
			sprintf(buf,"enseash [exit:%d|%ld] %%~ >",WEXITSTATUS(status),(long)((end.tv_sec-start.tv_sec)*1000+(end.tv_nsec-start.tv_nsec)/1000000));
			write(STDOUT_FILENO, buf, sizeof(buf));
		}else if (WIFSIGNALED(status)){
			sprintf(buf,"enseash [sign:%d|%ld] %%~ >",WTERMSIG(status),(long)((end.tv_sec-start.tv_sec)*1000+(end.tv_nsec-start.tv_nsec)/1000000));
			write(STDOUT_FILENO, buf, sizeof(buf));
		}		
		
	}	
}
