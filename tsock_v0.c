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









void main (int argc, char **argv)
{
    int sock;
    int port=atoi(argv[argc-1]);
    char * message=malloc(30*sizeof(char));
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
		    exit(1);
		case 'u':
		    protocole=1;
		    break;
		case 'p':
			if (source == 1) {
				printf("usage: cmd [-p|-s][-n ##]\n");
				exit(1);
			}
			source = 0;
			break;

		case 's':
			if (source == 0) {
				printf("usage: cmd [-p|-s][-n ##]\n");
				exit(1) ;
			}
			source = 1;
			break;

		case 'n':
			nb_message = atoi(optarg);
			break;

		default:
			printf("usage: cmd [-p|-s][-n ##]\n");
			break;
		}
	}

	if (source == -1) {
		printf("usage: cmd [-p|-s][-n ##]\n");
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
	} else {
		if (source == 1) {
			nb_message = 10 ;
			printf("nb de tampons à envoyer = 10 par défaut\n");
		} else
		printf("nb de tampons à envoyer = infini\n");

	}
	if (protocole==0){
	    printf("Protocole TCP\n");
    }
	else{
	    printf("Protocole UDP\n");
	}
	if (protocole==1){
	    if (source==1){
	        struct hostent *hp;
	        struct sockaddr_in adr_distant;
	        if((sock=socket(AF_INET,SOCK_DGRAM,0))==-1){
	            printf("échec de création du socket\n");
	            exit(1);
	        }
	        memset((char *)& adr_distant, 0, sizeof(adr_distant)) ;
            adr_distant.sin_family = AF_INET ; /* domaine Internet*/
            adr_distant.sin_port = htons(port) ;  
            
            /* Adresse IP */
	        if((hp=gethostbyname(argv[argc-2]))==NULL){
	            printf("erreur gethostbyname\n") ;
	            exit(1) ;
	        }   
	        memcpy( (char*)&(adr_distant.sin_addr.s_addr),hp->h_addr,hp->h_length ); 
            for(int i=0; i<nb_message+1; i++){
                sprintf(message,"%5d",i+1);
                construire_message(message+5,(char)(i%26)+97, lg-5);
                if(sendto(sock,message,lg,0,(struct sockaddr_in *)&adr_distant, sizeof(adr_distant))==-1){
                    printf("erreur sendto\n");
                    exit(1);
                }
                printf("SOURCE: Envoie n°%d (%d) [%s]\n",i+1,lg, message);
                
            }
       } 
        /*if(source==0){
            if((sock=socket(AF_INET,SOCK_DGRAM,0))==-1*/
    }
}
