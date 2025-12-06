Pour compiler les programmes :
    -"make -B tpEmRec" pour avoir les exécutables éméteur et récepteur
    -"make -B tpPeer" pour avoir l'exécutable routP

Pour exécuter:
    -Processus éméteur:
        ./routPem [IP] [mon_num_routeur] [num_routeur_voisin]

    -Processus récepteur:
        ./routPrec [IP] [mon_num_routeur] [num_routeur_voisin]

    -Processus routP :
        ./routP [IP] [mon_num _routeur] [num_routeur_voisin]

Information pour le Processus routP:

    comme montré dans la figure 5 du sujet du TP4, lorsqu'on lance le Processus il envoie ses entrées
    au Processus voisin puis il recoit les entrées du voisin qui aura envoyé ses entrées, lorsque le Processus
    voisin est lancé il envoie ses entrées au Processus initial qui les recevras et mettra à jour ses entrées.

    pour tester ce programme il faut lancer dans un premier temps ./routP [IP] [mon_num _routeur] [num_routeur_voisin]
    puis dans un nouveau terminal ./routP [IP] [num_routeur_voisin] [mon_num _routeur] ce qui permettra de voir le
    premier processus remplir sa table de routage.