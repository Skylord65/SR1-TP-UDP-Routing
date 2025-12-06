/*=============================================================*/
//Programme simulant un protocole de routage dynamique simplifié
// Ce programme code uniquement le comportement
// d'émetteur d'une annonce de routage
// vers UN SEUL routeur voisin pour UN échange initial de routes
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

#define BUF_SIZE_OUT 64 // we should send less...
#define IPV4_ADR_STRLEN 16  // == INET_ADDRSTRLEN
#define LOCALHOST "127.0.0.1"
#define NO_BASE_PORT 17900  // base number for computing real port number


/* =================================================================== */
/* FONCTION PRINCIPALE : PEER PROCESSUS DE ROUTAGE ROLE EMETTEUR ONLY  */
/* =================================================================== */

int main(int argc, char **argv) {

  // Usage routPem IDIP@ssRouter  MyNumberRouter NeigborNumberRouter
  // Example routPem 10.1.1.1 1 2

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

  /* init caracteristiques serveur distant (struct sockaddr_in) */

  struct sockaddr_in skv_addr;
  memset(&skv_addr, 0, sizeof(skv_addr));
  skv_addr.sin_family = AF_INET;
  skv_addr.sin_port = htons(17900+atoi(argv[3]));
  inet_pton(AF_INET, argv[1], &(skv_addr.sin_addr));

  int lenTable = myRoutingTable.nb_entry;

  /* evoie du nombre d'entrée dans la table */

  char msg[41];
  snprintf(msg, sizeof(msg), "%d", myRoutingTable.nb_entry);
  sendto(sid, msg, sizeof(msg), 0, (struct sockaddr *)&skv_addr, sizeof(skv_addr));
  printf("Annonce du nombre d'entrée envoyées : %s\n", msg);

  /* envoie du contenu de la table de routage */

  for ( int i = 0; i<lenTable; i++){
    snprintf(msg,sizeof(msg),"%s", myRoutingTable.tab_entry[i]);
    sendto(sid, &msg,  sizeof(msg), 0, (struct sockaddr *)&skv_addr, sizeof(skv_addr));
    printf("Annonce envoyée : %s\n", msg);
  }

  close(sid);
  exit(EXIT_SUCCESS);
  }
