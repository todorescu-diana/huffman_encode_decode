#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "decode.h"

unsigned char bit_buffer_dec;       // buffer pe 1 byte ce ajuta in scrierea bitilor in fisierul out pt ca rezolutia in prelucratul fisierelor in C este de 1 byte
int bit_nr_biti_retinuti_dec = 0;   // contor care numara cati biti sunt retinuti in buffer-ul de 8 biti

char **coduri_huffman_matrice; // matrice in care se vor stoca codurile huffman ale caracterelor, va fi alocata dinamic

FILE *fin;  // pointer fisier input
FILE *fout; // pointer fisier output

NodReconstruire* radacina_reconstr; // radacina arbore ce urmeaza sa fie reconstruit

char preordine[128]; // vector in care se va retine preordinea citita in forma (2) - mai jos
char preordine_caractere[128]; // vector  in care se vor retine doar caracterele din preordine (fara elementele '-', care reprezinta noduri neterminale)

int numar_noduri; // retine cate noduri avem de adaugat in arborele huffman ce va fi creat din informatia continuta in header

int caracter_de_adaugat = 0;        // "boolean" 0/1 - flag ce este pus pe 1 in momentul in care in preordinea citita din header-ul fisierului
                                    // de intrare este citit un bit 1 ( => ca urmatorii 8 biti vor reprezenta valoarea unui caracter frunza in arbore);
                                    // arborele este stocat in header cu ajutorul preordinii lui, care e de forma "0001a1b01c,
                                    // 0 = nod neterminal, 1 = marcheaza ca urmatorii 8 biti vor reprezenta valoarea ASCII a unui nod frunza
int caractere_adaugate = 0; // initializare contor caractere adaugate in arbore

int idx_preordine = 0;  // contor caractere adaugate in preordine[], incluzand caracterele ce nu sunt noduri terminale in arborele huffman (in cazul prezent '-')
int idx_preordine_caractere = 0;    // contor ce retine exclusiv nr de caractere noduri terminale in arborele huffman

int arr[100], varf; // folosite in functia huffman afisare coduri

unsigned char buffer; // buffer de 1 byte in care vom citi informatiile din fisier
int bits[8]; // byte buffer de 0 | 1 intregi

char *cod_buffer; // vector de char ce va fi alocat dinamic pentru a retine nr_max_biti returnat din functia de mai sus;
                  // va retine cate un bit citit din textul encoded pana cand codul retinut in cod_buffer este egal cu
                  // unul dintre codurile retinute in matricea de coduri huffman

int sfarsit;

int varf = 0; // folosite in functia huffman afisare coduri

int Nod_este_frunza_dec(NodReconstruire* radacina) { // functie care verifica daca nodul dat ca parametru este frunza

    return !(radacina -> st) && !(radacina -> dr);
}

NodReconstruire* NodReconstruire_nod_nou(char ch) { // functie pt crearea unui nod nou de adaugat in arbore
    NodReconstruire* nod = (NodReconstruire*)malloc(sizeof(NodReconstruire));
    nod->ch = ch;
    nod->st = nod->dr = NULL;

    return nod;
}

void parcurgere_inordine_dec(NodReconstruire* radacina) { // functie parcurgere inordine arbore dat ca parametru
    if (radacina == NULL) {
        return;
    }

    parcurgere_inordine_dec(radacina->st);
    printf("%c ", radacina->ch);
    parcurgere_inordine_dec(radacina->dr);
}

int ch_find_index_in_vector_preordine(char ch) { // functie care returneaza index-ul caracterului dat ca parametru in vector_frecventa
    for(int i=0; i < numar_noduri; i++) {
        if(ch == preordine_caractere[i]) return i;
    }
    return -1;  // caracterul nu a fost gasit in vector_frecventa
}

void cod_stocare_afisare_dec(int arr[], int n, char ch) { // functie pt stocarea si afisarea unui cod de n caractere
    coduri_huffman_matrice = (char**) realloc ( coduri_huffman_matrice, numar_noduri * sizeof(char*) ); // alocare dinamica "linii" in "matrice"

    coduri_huffman_matrice[ ch_find_index_in_vector_preordine(ch) ] = (char*) malloc ( (n + 1)* sizeof(char) ); //alocare dinamica "coloane" in "matrice"

    int i;
    for (i = 0; i < n; i++){
        coduri_huffman_matrice[ch_find_index_in_vector_preordine(ch)][i] = arr[i] + '0'; // " + '0' " <=> transformare arr[i] din intreg in char
    }

    coduri_huffman_matrice[ch_find_index_in_vector_preordine(ch)][i] = '\0';

    printf("%s", coduri_huffman_matrice[ch_find_index_in_vector_preordine(ch)]);

    printf("\n");
}

void HuffmanTree_afisare_coduri_dec(NodReconstruire* radacina, int arr[], int varf) { // functie ce printeaza si stocheaza codurile huffman de la radacina - create cu ajutorul arborelui huffman;
                                                                                     // foloseste vectorul arr[] pt stocarea codurilor

    if (radacina -> st) { // asignare 0 pe partea stanga
        arr[varf] = 0;
        HuffmanTree_afisare_coduri_dec(radacina -> st, arr, varf + 1);
    }

    if (radacina -> dr) { //asignare 1 pe partea dreapta
        arr[varf] = 1;
        HuffmanTree_afisare_coduri_dec(radacina -> dr, arr, varf + 1);
    }

    if (Nod_este_frunza_dec(radacina)) { // daca nodul radacina dat este frunza => contine un caracter => afisare caracter + cod stocat in arr[]
        printf("%c: ", radacina -> ch);
        cod_stocare_afisare_dec(arr, varf, radacina -> ch);
    }
}

NodReconstruire* arbore_construire_preordine(char *sir, int *idx) {  // functie ce recreaza arborele huffman dupa preordinea data in header-ul fisierului binar input
    NodReconstruire* nod = (NodReconstruire*) malloc ( sizeof(NodReconstruire) );
    nod -> ch = sir[*idx];
    nod -> children = 0;

    (*idx)++;

    if(nod -> ch == '-') {
        if(nod -> children == 0 && (*idx <= sfarsit -1)) {
                nod -> st = arbore_construire_preordine(sir, idx);
                nod -> children ++;
        }
        if(nod -> children == 1 && (*idx <= sfarsit)) {
            nod -> dr = arbore_construire_preordine(sir, idx);
            nod -> children ++;
        }
    }
    else {
        nod -> st = NULL;
        nod -> dr = NULL;
    }

    if(nod -> children == 2 || nod->st == NULL) return nod;
}

void unsigned_char_to_int(unsigned char c) {    // functie ce ia un unsigned char c ca parametru si ii retine bitii in array-ul de int 0/1 bits[8]
    for (int i = 0 ; i != 8 ; i++) {
        bits[i] = (c & (1 << i)) != 0;
    }
}

int binary_to_int(int *bits) {  // returneaza un intreg obtinut dintr-un vector de 0 sau 1 ce retine reprezentarea binara
    int intreg = 0;

    for(int i=0; i<8; i++) {
        intreg += (pow(2,i) * bits[i]);
    }

    return intreg;
}

void bit_scriere_bit_dec(int bit) { // functie ce scrie bit-ul 0/1 dat ca parametru in buffer-ul bit_buffer pe 1 byte
   bit_buffer_dec <<= 1;            // shiftare la stanga bit buffer pentru a face loc bitului nou
    if (bit) bit_buffer_dec |= 0x1;  // daca bitul de scris este 1, setam LSB-ul bit bufferului pe 1

    bit_nr_biti_retinuti_dec++;        // incrementare contor biti in buffer

    if (bit_nr_biti_retinuti_dec == 8) // daca byte-ul buffer este ocupat complet, se adauga in preordine[]
    {
        preordine[idx_preordine++] = bit_buffer_dec;
        if(bit_buffer_dec != '-') preordine_caractere[idx_preordine_caractere++] = bit_buffer_dec;
        caractere_adaugate++;
        bit_nr_biti_retinuti_dec = 0;
        bit_buffer_dec = 0;
        caracter_de_adaugat = 0;    // s-a adaugta caracterul in preordine[] => caracter_de_adagugat readus la valoarea default 0
    }
}

int inaltime_arbore(NodReconstruire* radacina) {     // functie ce returneaza inaltimea arboelui huffman <=> lungimea celui mai lung cod
    int h_st = 0, h_dr = 0;

    if(radacina == NULL) return 0;

    h_st = inaltime_arbore(radacina->st);
    h_dr = inaltime_arbore(radacina->dr);

    return (h_st > h_dr)? h_st + 1 : h_dr + 1;
}

void huffman_decode(char *nume_fisier_intrare) {
    char *fisier_out_nume;     // stocheaza numele fisierului de scris text output ( fisier_in_nume - ".huffman" )

    fisier_out_nume = (char*) malloc ( (strlen(nume_fisier_intrare) + 14) * sizeof(char) );     // +13 = 13 caractere suplimentare in extensia .decompressed, +1 = caracterul terminal '\0'

    fin = fopen(nume_fisier_intrare, "rb");

    strcpy(fisier_out_nume, nume_fisier_intrare);
    strcat(fisier_out_nume, ".decompressed");

    fout = fopen(fisier_out_nume, "w");

    if (fin == NULL || fout == NULL) {
        printf("Eroare deschidere fisiere.\n");
    }

    if ((fread(&buffer, sizeof(buffer), 1, fin)) > 0) {   // citire din fisier 1 byte = numarul de caractere ce il contine arborele huffman ce trebuie reconstruit
        unsigned_char_to_int(buffer);

        numar_noduri = binary_to_int(bits);
    }

    while ( caractere_adaugate < numar_noduri ) { // citire cate 1 byte = preordine arbore + creare arbore;
                                                  // se citeste in buffer atata timp cat nr de bytes cititi (ce returneaza fread) este > 0 SI
                                                  // caractere_adaugate <= nr_noduri
        if(fread(&buffer, sizeof(buffer), 1, fin) > 0) {  // preordinea citita va fi de forma(1) 0001c1a01e,
                                                          // caracterele fiind si ele reprezentate pe biti dupa ASCII;

        for(int i=7; i>=0; i--) {   // aducem preordinea citita astfel la forma (2) ---ca-e, - fiind un nod neterminal, c, a si e fiind
                                    // noduri frunza cu valori ale caracterelor
            if(caracter_de_adaugat == 0 && (caractere_adaugate != numar_noduri)) {  // nu s-a intalnit niciun 1 pana acum, deci nu urmeaza niciun cod ASCII al vreunui caracter de adaugat
                if ( (buffer & (1 << i)) == 0 ) {       // nod neterminal
                    preordine[idx_preordine++] = '-';
                }
                else {  // daca se citeste 1, => avem "1 + reprezentarea ASCII a caracterului"; reprezentarea ASCII a caracterului e pe 8 biti => va trebui sa avansam citirea din fisier cu inca 1 byte dupa ce retinem bitii pe care ii avem deocamdata din reprezentarea ASCII
                    caracter_de_adaugat = 1;
                    continue;
                }
            }
            else{   // citire cod ASCII in buffer_ch
                if ( (buffer & (1 << i)) == 0 ) {
                    bit_scriere_bit_dec(0);
                }
                else {
                    bit_scriere_bit_dec(1);
                }
            }
        }
        }
        else break;
    }       // am terminat de citit preordinea arborelui.
    printf("PREORDINE CITITA DIN HEADER:\n");

    printf("%s", preordine);
    printf("\n\n\n");

    sfarsit = idx_preordine-1;
    int idx = 0;

    radacina_reconstr = arbore_construire_preordine(preordine, &idx); // reconstruire arbore dupa preordinea citita anterior

    printf("INORDINE ARBORE RECONSTRUIT:\n");
    parcurgere_inordine_dec(radacina_reconstr); // am parcurs in inrodine pt a verifica ca e corect reconstruit

    printf("\n\n\nCODURI:\n");

    HuffmanTree_afisare_coduri_dec(radacina_reconstr, arr, varf); // afisare coduri huffman. codurile sunt stocate in coduri_huffman_matrice[ ch_find_index_in_vector_preordine(ch) ]

    // urmeaza citirea textului encoded si decodarea lui

    cod_buffer = (char*)malloc( (inaltime_arbore(radacina_reconstr)+1) * sizeof(char)); // +1 pentru terminatorul de sir '\0'

    int cod_nr_biti_adaugati = 0; // variabila ce retine numarul de biti adaugati in cod_buffer

    printf("\n\nTEXT DECODED:\n");
    int exit  = 0;

    while ( fread(&buffer, sizeof(buffer), 1, fin) > 0 && !exit) {   // citire cate 1 byte = text encoded; se citeste in buffer atata timp cat nr de bytes cititi (ce returneaza fread) este > 0)                                           //aducem preordinea citita astfel la forma (2) ---ca-e, - fiind un nod neterminal, c, a si e fiind noduri frunza cu valori ale caracterelor
    for(int i=7; i>=0; i--) {

            if ( (buffer & (1 << i)) == 0 ) {
                cod_buffer[cod_nr_biti_adaugati++] = '0';
            }
            else {
                cod_buffer[cod_nr_biti_adaugati++] = '1';
            }
            cod_buffer[cod_nr_biti_adaugati] = '\0';
            for(int j=0; j<numar_noduri; j++) {
                if( strcmp(cod_buffer, coduri_huffman_matrice[j]) == 0 ) {  // a fost gasit un caracter => il afisam si scriem in fisier out.
                    if(preordine_caractere[j] == '&') {
                        exit = 1;break;}
                    else {
                        printf("%c", preordine_caractere[j]);
                        fwrite(&preordine_caractere[j], 1, sizeof(preordine_caractere[j]), fout);
                        cod_nr_biti_adaugati = 0;
                    }
                }
            }
            if(exit == 1) break;
        }
    }
    printf("\n");

    for(int i=0; i<numar_noduri; i++) {     // eliberare memorie alocata dinamic & inchidere fisiere
        free(coduri_huffman_matrice[i]);
    }
    free(coduri_huffman_matrice);
    free(cod_buffer);

    fclose(fin);
    fclose(fout);
    free(fisier_out_nume);
}
