/* librairie standard ... */
#include <stdlib.h>
/* pour getopt */
#include <unistd.h>
/* déclaration des types de base */
#include <sys/types.h>
/* constantes relatives aux domaines, types et protocoles */
#include <sys/socket.h>
/* constantes et structures propres au domaine UNIX */
#include <sys/un.h>
/* constantes et structures propres au domaine INTERNET */
#include <netinet/in.h>
/* structures retournées par les fonctions de gestion de la base de
données du réseau */
#include <netdb.h>
/* pour les entrées/sorties */
#include <stdio.h>
/* pour la gestion des erreurs */
#include <errno.h>


void construire_message(char *message, char motif, int lg) {
    int i;
    for (i=0;i<lg;i++) message[i] = motif;
}


void afficher_message(char *message, int lg) {
    int i;
    printf("message construit : ");
    for (i=0;i<lg;i++) printf("%c", message[i]);
    printf("\n");
}









void main (int argc, char **argv){
	int sock;
	int port=atoi(argv[argc-1]);
	char * message;
	int c;
	extern char *optarg;
	extern int optind;
	int nb_message = -1; /* Nb de messages à envoyer ou à recevoir, par défaut : 10 en émission, infini en réception */
	int source = -1 ; /* 0=puits, 1=source */
	int protocole=0; /* 0=TCP 1=UDP */
	int lg=30 ;/* longueur du message*/
	while ((c = getopt(argc, argv, "pn:sul:")) != -1) {
		switch (c) {
		case 'l':
			lg=atoi(optarg);
			break;
		case 'u':
			protocole=1;
			break;
		case 'p':
			if (source == 1) {
				printf("usage: cmd [-p|-s][-u][-n ##][-l ##]\n");
				exit(1);
			}
			source = 0;
			break;
		case 's':
			if (source == 0) {
				printf("usage: cmd [-p|-s][-u][-n ##][-l ##]\n");
				exit(1) ;
			}
			source = 1;
			break;
		case 'n':
			nb_message = atoi(optarg);
			break;
		default:
			printf("usage: cmd [-p|-s][-u][-n ##][-l ##]\n");
			break;
		}
	}

	if (source == -1) {
		printf("usage: cmd [-p|-s][-u][-n ##][-l ##]\n");
		exit(1) ;
	}

	if (source == 1)
		printf("on est dans le source\n");
	else
		printf("on est dans le puits\n");

	if (nb_message != -1) {
		if (source == 1)
			printf("nb de tampons à envoyer : %d\n", nb_message);
		else
			printf("nb de tampons à recevoir : %d\n", nb_message);
	} 
	else {
		if (source == 1) {
			nb_message = 10 ;
			printf("nb de tampons à envoyer = 10 par défaut\n");
		} else
		printf("nb de tampons à recevoir= infini\n");

	}
	if (protocole==0){
	    printf("Protocole TCP\n");
    }
	else{
	    printf("Protocole UDP\n");
	}
	message=malloc(lg*sizeof(char));
//------------ UDP---------------
	if (protocole==1){
//-----------------------SOURCE----------------	
		if (source==1){
	  		struct hostent *hp;
	    	struct sockaddr_in adr_distant;
	    	if((sock=socket(AF_INET,SOCK_DGRAM,0))==-1){
	    		printf("échec de création du socket\n");
		      exit(1);
		    }
		    memset((char *)& adr_distant, 0, sizeof(adr_distant)) ;      			 				adr_distant.sin_family = AF_INET; /* domaine Internet*/
      		adr_distant.sin_port = htons(port);  
// Adresse IP 
	    	if((hp=gethostbyname(argv[argc-2]))==NULL){
	    		printf("erreur gethostbyname\n");
	    		exit(1) ;
			}   
	    	memcpy( (char*)&(adr_distant.sin_addr.s_addr),hp->h_addr,hp->h_length ); 
			for(int i=0; i<nb_message; i++){
      			sprintf(message,"%5d",i+1);
      			construire_message(message+5,(char)(i%26)+97, lg-5);
        		if(sendto(sock,message,lg,0,(struct sockaddr *)&adr_distant, sizeof(adr_distant))==-1){
        			printf("erreur sendto\n");
          			exit(1);
				}
        		printf("SOURCE: Envoie n°%d (%d) [%s]\n",i+1,lg, message);
			}
		}//FINSOURCE
//-----------------------PUIT----------------			
		else{
			int * plg_adr_em;
			struct sockaddr_in adr_local;
			if((sock=socket(AF_INET,SOCK_DGRAM,0))==-1){
      			printf("Échec de création du socket\n");
				exit(1);
      		}
      		memset((char*)&adr_local, 0, sizeof(adr_local)) ;
      		adr_local.sin_port = htons(port);
      		adr_local.sin_addr.s_addr=INADDR_ANY;
      		adr_local.sin_family=AF_INET;
      		if (bind(sock,(struct sockaddr *)&adr_local,sizeof(adr_local))==-1){
      			printf("Échec du bind\n");
      			exit(1);
      		}
      		int i=0;
      		if (nb_message==-1){
      			while(1){
      				if(recvfrom(sock,message,lg,0,(struct sockaddr *)&adr_local,plg_adr_em)==-1){
      					printf("Échec lors de la réception");
      					exit(1);
      				}
      				printf("PUITS: Reception n°%d (%d) [%s]\n",i+1,lg,message);
      				i++;
      			}
      		}
      		else{
      			while(i<nb_message){
      				if(recvfrom(sock,message,lg,0,(struct sockaddr *)&adr_local,plg_adr_em)==-1){
      					printf("Échec lors de la réception");
      					exit(1);
      				}
      				printf("PUITS: Reception n°%d (%d) [%s]\n",i+1,lg,message);
      				i++;
      			}    
	  		}
		}//FINPUIT
	}//FINUDP
//--------------------TCP------------------------
	else{
		//--------------PUIT------------------
		if (source==0){
			int sock2;
			int * plg_adr_distant=malloc(sizeof(int));
			struct sockaddr_in adr_local;
			struct sockaddr_in adr_distant;
			if ((sock=socket(AF_INET, SOCK_STREAM,0))==-1){
				printf("Échec de création du socket\n");
				exit(1);
			}
			memset((char*)&adr_local, 0, sizeof(adr_local));
			adr_local.sin_port=htons(port);
			adr_local.sin_addr.s_addr=htonl(INADDR_ANY);
			adr_local.sin_family=AF_INET;
			if (bind(sock,(struct sockaddr *)&adr_local,sizeof(adr_local))==-1){
      			printf("Échec du bind\n");
      			exit(1);
      		}
			if (listen(sock,5)==-1){
				printf("Échec listen\n");
				exit(1);
			}
			if ((sock2=accept(sock,(struct sockaddr *)&adr_distant, plg_adr_distant))==-1){
				printf("Échec accept\n");
				exit(1);
			}

			int i=0;
			int x ;
			if (nb_message==-1){
      			while((x=recv(sock2,message,lg,0))!=0){
     				if(x==-1){
      					printf("Échec lors de la réception\n");
      					exit(1);
      				}
      				printf("PUITS: Reception n°%d (%d) [%s]\n",i+1,lg,message);
      				i++;		
      			}
      		}
      		else{
      			while(i<nb_message){
      				if(recv(sock2,message,lg,0)==-1){
      					printf("Échec lors de la réception\n");
      					exit(1);
      				}
      				else{
      				    printf("PUITS: Reception n°%d (%d) [%s]\n",i+1,lg,message);
      				    i++;
      				}
      			}    
	  		}
		}//FIN PUIT
		//-------------------------SOURCE-------------------------------
		else{
			struct sockaddr_in adr_distant;
			if((sock=socket(AF_INET, SOCK_STREAM,0))==-1){
				printf("Échec de création du socket\n");
				exit(1);
			}
			memset((char*)&adr_distant,0,sizeof(adr_distant));
			adr_distant.sin_port=htons(port);
			adr_distant.sin_addr.s_addr=INADDR_ANY;
			adr_distant.sin_family=AF_INET;
			if(connect(sock,(struct sockaddr*)&adr_distant,sizeof(adr_distant))==-1){
				printf("Échec connect\n");
				exit(1);
			}
			for(int i=0; i<nb_message; i++){
      			sprintf(message,"%5d",i+1);
      			construire_message(message+5,(char)(i%26)+97, lg-5);
        		if(write(sock,message,lg)==-1){
        			printf("Échec lors de l'envoie\n");
          			exit(1);
				}
        		printf("SOURCE: Envoie n°%d (%d) [%s]\n",i+1,lg, message);
			}
		}//FIN SOURCE		 
	}//FIN TCP	
	close(sock);
}
