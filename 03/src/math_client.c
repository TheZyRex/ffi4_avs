#include "math.h"
#include <rpc/rpc.h>

int main(int argc, char *argv[])
{
    if (argc != 4) {
        fprintf(stderr, "Verwendung: %s <Server-Adresse> <Zahl1> <Zahl2>\n", argv[0]);
        exit(1);
    }

    CLIENT *cl; // Client-Handle
    intpair numbers; // Struktur für die beiden Zahlen
    int *result; // Zeiger auf das Ergebnis

    // Erzeugen des Client-Handles
    cl = clnt_create(argv[1], MATHPROG, MATHVERS, "tcp");
    if (cl == NULL) {
        clnt_pcreateerror(argv[1]);
        exit(1);
    }

    // Zahlen aus den Kommandozeilenargumenten holen
    numbers.a = atoi(argv[2]);
    numbers.b = atoi(argv[3]);

    // Aufruf der add_1-Prozedur auf dem Server
    result = add_1(&numbers, cl);
    if (result == NULL) {
        clnt_perror(cl, "Fehler beim Aufruf der add_1-Prozedur");
        exit(1);
    }

    // Ausgabe des Ergebnisses
    printf("Ergebnis der Addition: %d\n", *result);

    // Freigeben des Client-Handles
    clnt_destroy(cl);

    return 0;
}