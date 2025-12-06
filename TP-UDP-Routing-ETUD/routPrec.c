/*================================================*/
//Programme simulant un protocole de routage dynamique simplifié
// Ce programme code uniquement le comportement
// de récpeteur d'une annonce de routage
// émise depuis UN SEUL routeur voisin pour UN échange initial de routes
// T. Desprats - Novembre 2022

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h> // struct sockaddr_in
#include <time.h>
#include <signal.h>
#include <stdbool.h>

#include "tabrout.h"

#define BUF_SIZE_IN 64 // we should receive less...
#define IPV4_ADR_STRLEN 16  // == INET_ADDRSTRLEN
#define LOCALHOST "127.0.0.1"
#define NO_BASE_PORT 17900  // base number for computing real port number


/* =================================================================== */
/* FONCTION PRINCIPALE : PEER PROCESSUS DE ROUTAGE ROLE RECEPTEUR ONLY */
/* =================================================================== */
int main(int argc, char **argv) {

  // Usage routPrec IDIP@ssRouter  MyNumberRouter NeigborNumberRouter
  // Example routPrec 10.1.1.1 1 2

  char idInitConfigFile [20]; //Id of the configuration file of the router
  char myId [32]; // String array representing the whole id of the Router

  routing_table_t myRoutingTable; //Routing TABLE



  /* Building ID Router from command args */
  sprintf(myId,"R%s %s",argv[2],argv[1]);
  printf("ROUTEUR : %s\n",myId );
  //printf("construction id fichier\n");
  /* Building Config File ID from command args */
  sprintf(idInitConfigFile,"R%sCfg",argv[2]);
  strcat(idInitConfigFile,".txt");
  //printf("\n Nom fichier Configuration : %s",idInitConfigFile);
  /* Loading My Routing Table from Initial Config file */
  init_routing_table(&myRoutingTable, idInitConfigFile);
  printf("ROUTEUR : %d entrées initialement chargées \n",myRoutingTable.nb_entry);
  display_routing_table(&myRoutingTable,myId);


  /* creation de socket */

  int sid;
  sid = socket(AF_INET, SOCK_DGRAM, 0);
    if (sid==-1)
    {
        perror("Socket cretion error");
        return EXIT_FAILURE;
    }

    /* init caracteristiques du socket (struct sockaddr_in) */

    struct sockaddr_in sk_addr;
    memset(&sk_addr, 0, sizeof(sk_addr));
    sk_addr.sin_family = AF_INET;
    sk_addr.sin_port = htons(17900+atoi(argv[2])); /* host to network byte order */
    sk_addr.sin_addr.s_addr = htonl(INADDR_ANY); /* any interface */

    /* association des identifiants */

    if (bind(sid, (struct sockaddr*) &sk_addr, sizeof(sk_addr)) == -1) {
        perror("Bind error");
        close(sid);
        return EXIT_FAILURE;
    }

  /* creation des information du voisin (ip, port, socket) */

  struct sockaddr_in voisin_addr;
  socklen_t voisin_len = sizeof(voisin_addr);
  char msg[41];

  /* recteption du nombre d'entrée dans la table du voisin */

  recvfrom(sid, msg, sizeof(msg), 0, (struct sockaddr *)&voisin_addr, &voisin_len);
  int Nb_entry = atoi(msg);
  printf("Nombre d'entrée du routeur voisin : %d\n", Nb_entry);

  /* reception des entrée de la table du voisin */

  for (int i = 0; i < Nb_entry; i++){
    recvfrom(sid, msg, sizeof(msg), 0, (struct sockaddr *)&voisin_addr, &voisin_len);
    printf("Annonce reçue : %s\n", msg);
    if (!(is_present_entry_table(&myRoutingTable,msg))){
      printf("ajout de l'entrée : %s\n", msg);
      add_entry_routing_table(&myRoutingTable, msg);
    }
  }

  close(sid);

  // Display new content of my routing table
  display_routing_table(&myRoutingTable,myId);
  exit(EXIT_SUCCESS);
 }
