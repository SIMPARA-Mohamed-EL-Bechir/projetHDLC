#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
typedef struct {
    char Ad[8];
    char Crtl[8];
    char Data[32];
    char FCS[16];
} Trame;
void typedetrame(Trame trame);
Trame initialisationTrame(Trame trame);
char * contenudelatrame(Trame trame);
void verificationData(Trame trame);

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
        Trame receivedTrame;
        read(p[0], buf, (sizeof(trame.Ad) + sizeof(trame.Crtl) + sizeof(trame.Data) + sizeof(trame.FCS)));
        memcpy(&receivedTrame, buf, sizeof(Trame));
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
        typedetrame(receivedTrame);
        printf("\n");

    }

    return 0;
}
void typedetrame(Trame trame){
    if(trame.Crtl[0] == '0'){
        printf("Il s'agit d'une trame d'information\n"); 
        if(trame.Crtl[4] == '0'){
            printf("Il s'agit du pull\n");
        } else {
            printf("Il s'agit du final\n");
        }
        char e[5]; 
        char r[5]; 
        int k = 0, j = 0;
        for(int i = 1; i < 4; i++){
            e[k] = trame.Crtl[i];
            k++;
        }
        e[4] = '\0'; 
        for(int i = 5; i < 8; i++){
            r[j] = trame.Crtl[i];
            j++;
        }
        r[4] = '\0'; 
        printf("La trame en cours est : %s\n", e);
        printf("La trame suivante est : %s\n", r);
        
    }
    if(trame.Crtl[0] == '1' && trame.Crtl[1] == '0'){
        printf("Il s'agit d'une trame de supervision\n");
        if(trame.Crtl[2] == '0' && trame.Crtl[3] == '0'){
            printf("Received and ready\n");
        } else if(trame.Crtl[2] == '0' && trame.Crtl[3] == '1'){
            printf("Rejet\n");
        } else if(trame.Crtl[2] == '1' && trame.Crtl[3] == '0'){
            printf("Received and not ready\n");
        } else {
            printf("Selective reject\n");
        }
        char e[5]; 
        int k = 0;
        for(int i = 5; i < 8; i++){
            e[k] = trame.Crtl[i];
            k++;
        }
        //e[4] = '\0'; 
        printf("La trame en cours est : %s\n", e);
    } else if(trame.Crtl[0] == '1' && trame.Crtl[1] == '1') {
        printf("Il s'agit d'une trame non numerotee\n");
        if(trame.Crtl[2] == '1' && trame.Crtl[3] == '1' && trame.Crtl[5] == '1' && trame.Crtl[6] == '0' && trame.Crtl[7] == '0'){
            printf("Le type de cette trame est SABM\n");
        }
        if(trame.Crtl[2] == '0' && trame.Crtl[3] == '0' && trame.Crtl[5] == '0' && trame.Crtl[6] == '1' && trame.Crtl[7] == '0'){
            printf("Le type de cette trame est DISC\n");
        }
        if(trame.Crtl[2] == '0' && trame.Crtl[3] == '0' && trame.Crtl[5] == '1' && trame.Crtl[6] == '1' && trame.Crtl[7] == '0'){
            printf("Le type de cette trame est UA\n");
        }
        if(trame.Crtl[2] == '1' && trame.Crtl[3] == '0' && trame.Crtl[5] == '1' && trame.Crtl[6] == '0' && trame.Crtl[7] == '1'){
            printf("Le type de cette trame est FRMR\n");
        }
        if(trame.Crtl[2] == '1' && trame.Crtl[3] == '1' && trame.Crtl[5] == '0' && trame.Crtl[6] == '0' && trame.Crtl[7] == '0'){
            printf("Le type de cette trame est DM\n");
        }
    }
}


Trame initialisationTrame(Trame trame){
    srand(time(NULL));
    for (int i = 0; i < 8; i++) {
        trame.Ad[i] = '1';
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
    if(trame.Crtl[0] == '1' && trame.Crtl[1] == '1') {
        if(trame.Crtl[2] == '1' && trame.Crtl[3] == '1' && trame.Crtl[5] == '1' && trame.Crtl[6] == '0' && trame.Crtl[7] == '0'){
            if(trame.Crtl[4] == '0'){
                trame.Crtl[4] =='1';
            }
        }
        if(trame.Crtl[2] == '0' && trame.Crtl[3] == '0' && trame.Crtl[5] == '0' && trame.Crtl[6] == '1' && trame.Crtl[7] == '0'){
                if(trame.Crtl[4] == '0'){
                trame.Crtl[4] =='1';
                }
            }
        if(trame.Crtl[2] == '0' && trame.Crtl[3] == '0' && trame.Crtl[5] == '1' && trame.Crtl[6] == '1' && trame.Crtl[7] == '0'){
                if(trame.Crtl[4] == '1'){
                trame.Crtl[4] =='0';
                }
            }
        if(trame.Crtl[2] == '1' && trame.Crtl[3] == '0' && trame.Crtl[5] == '1' && trame.Crtl[6] == '0' && trame.Crtl[7] == '1'){
                if(trame.Crtl[4] == '1'){
                trame.Crtl[4] =='0';
                } 
        }
        if(trame.Crtl[2] == '1' && trame.Crtl[3] == '1' && trame.Crtl[5] == '0' && trame.Crtl[6] == '0' && trame.Crtl[7] == '0'){
                if(trame.Crtl[4] == '1'){
                    trame.Crtl[4] =='0';
                }
        }
    }
    return trame;
}
char * contenudelatrame(Trame trame){
    char *adCtrlDataFCS = malloc(sizeof(trame.Ad) + sizeof(trame.Crtl) + sizeof(trame.Data) + sizeof(trame.FCS));
    int j = 0, k = 0, l = 0;
    for(int i = 0; i < sizeof(trame.Ad) + sizeof(trame.Crtl) + sizeof(trame.Data) + sizeof(trame.FCS); i++){
        if(i< sizeof(trame.Ad)){
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
    return adCtrlDataFCS;
}

void verificationData(Trame trame) {
    int compteur = 0;
    int j = 0;
    char data_verifiee[sizeof(trame.Data)];

    for (int i = 0; i < sizeof(trame.Data); i++) {
        if (trame.Data[i] == '1') {
            compteur++;
            data_verifiee[j++] = trame.Data[i];
            if (compteur == 5) {
                if (trame.Data[i + 1] == '0') {
                    i++;
                } else {
                    printf("Erreur : Absence de 0 apres 5 bits de 1 dans les donnees.\n");
                    return;
                }
                compteur = 0;
            }
        } else {
            compteur = 0;
            data_verifiee[j++] = trame.Data[i];
        }
    }

    printf("Donnees verifiees :\n");
    for (int i = 0; i < j; i++) {
        printf("%c", data_verifiee[i]);
    }
    printf("\n");
}