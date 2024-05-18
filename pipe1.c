#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
    char Ad[8];
    char Crtl[8];
    char Data[32];
    char FCS[16];
} Trame;

int typedetrame(Trame trame);
Trame initialisationTrame(Trame trame);
char * contenudelatrame(Trame trame);

int main() {
    int p[2];
    Trame trame;
    char buf[sizeof(trame.Ad) + sizeof(trame.Crtl) + sizeof(trame.Data) + sizeof(trame.FCS)];
    char *adCtrlDataFCS;

    trame = initialisationTrame(trame);

    pipe(p);

    if (fork()) {  
        close(p[0]);
        adCtrlDataFCS = contenudelatrame(trame);
        write(p[1], adCtrlDataFCS, (sizeof(trame.Ad) + sizeof(trame.Crtl) + sizeof(trame.Data) + sizeof(trame.FCS)));
        close(p[1]);
        free(adCtrlDataFCS);
    } else {  
        close(p[1]);

        read(p[0], buf, (sizeof(trame.Ad) + sizeof(trame.Crtl) + sizeof(trame.Data) + sizeof(trame.FCS)));
        close(p[0]);

        printf("\n Adresse: ");
        for (int i = 0; i < sizeof(trame.Ad); i++) {
            printf("%c", buf[i]);
        }
        printf("\n");
        printf("\n Controle: ");
        for (int i = sizeof(trame.Ad); i < sizeof(trame.Ad) + sizeof(trame.Crtl); i++) {
            printf("%c", buf[i]);
        }
        printf("\n");
        printf("\n Data: ");
        for (int i = sizeof(trame.Ad) + sizeof(trame.Crtl); i < (sizeof(trame.Ad) + sizeof(trame.Crtl) + sizeof(trame.Data)); i++) {
            printf("%c", buf[i]);
        }
        printf("\n");
        printf("\n FCS: ");
        for (int i = sizeof(trame.Ad) + sizeof(trame.Crtl) + sizeof(trame.Data); i < (sizeof(trame.Ad) + sizeof(trame.Crtl) + sizeof(trame.Data) + sizeof(trame.FCS)); i++) {
            printf("%c", buf[i]);
        }
        printf("\n");

        // Appel de la fonction typedetrame
        int type = typedetrame(trame);
        if(type == 1){
            printf("Trame d'Information\n");
        }
        else if(type == 2){
            printf("Trame de supervision\n");
        }
        else{
            printf("Trame non numerotée\n");
        }

    }

    return 0;
}

int typedetrame(Trame trame){
    if(trame.Crtl[sizeof(trame.Crtl)-1] == '0'){
        return 1;
    }
    else if(trame.Crtl[sizeof(trame.Crtl)-1] == '1'){
        return 2;
    }
    else{
        return 3;
    }
}

Trame initialisationTrame(Trame trame){
    // Initialisation du générateur de nombres aléatoires
    srand(time(NULL));
    for (int i = 0; i < 8; i++) {
        trame.Ad[i] = '0' + rand() % 2;
    }
    for (int i = 0; i < 8; i++) {
        trame.Crtl[i] = '0' + rand() % 2;
    }
    for (int i = 0; i < 32; i++) {
        trame.Data[i] = '0' + rand() % 2;
    }
    for (int i = 0; i < 16; i++) {
        trame.FCS[i] = '0' + rand() % 2;
    }
    return trame;
}

char * contenudelatrame(Trame trame){
    char *adCtrlDataFCS = malloc(sizeof(trame.Ad) + sizeof(trame.Crtl) + sizeof(trame.Data) + sizeof(trame.FCS) + 1); // Ajout de 1 pour le caractère nul de fin de chaîne
    int j = 0, k = 0, l = 0;
    for(int i = 0; i < sizeof(trame.Ad) + sizeof(trame.Crtl) + sizeof(trame.Data) + sizeof(trame.FCS); i++){
        if(i < sizeof(trame.Ad)){
            adCtrlDataFCS[i] = trame.Ad[i];
        }
        else if(i < sizeof(trame.Ad) + sizeof(trame.Crtl)){
            adCtrlDataFCS[i] = trame.Crtl[j];
            j++;
        }
        else if(i < sizeof(trame.Ad) + sizeof(trame.Crtl) + sizeof(trame.Data)){
            adCtrlDataFCS[i] = trame.Data[k];
            k++;
        }
        else{
            adCtrlDataFCS[i] = trame.FCS[l];
            l++;
        }
    }
    adCtrlDataFCS[sizeof(trame.Ad) + sizeof(trame.Crtl) + sizeof(trame.Data) + sizeof(trame.FCS)] = '\0'; // Ajout du caractère nul de fin de chaîne
    return adCtrlDataFCS;
}
