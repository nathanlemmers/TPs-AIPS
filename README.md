# Noms et Prénoms:
LEMMERS Nathan
VACHER Melvin
# Compilation
gcc AIPS.c -o tsock

# Execution en tant que puit
./tsock -p [-options] port
# Execution en tant que source
./tsock -s [-options] host port

# Options :
-n # : Nombre de messages emis ou reçus au maximum, 10 par defaut en émission et infini en réception.

-l # : Longueur du message emis ou reçu, 30 par défaut.

-u : Utilisation du protocole UDP, le protocole TCP est utilisé par défaut.
