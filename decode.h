#ifndef DECODE_H
#define DECODE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern unsigned char bit_buffer_dec;       // buffer pe 1 byte ce ajuta in scrierea bitilor in fisierul out pt ca rezolutia in prelucratul fisierelor in C este de 1 byte
extern int bit_nr_biti_retinuti_dec;   // contor care numara cati biti sunt retinuti in buffer-ul de 8 biti

extern char **coduri_huffman_matrice; // matrice in care se vor stoca codurile huffman ale caracterelor, va fi alocata dinamic

extern FILE *fin;  // pointer fisier input
extern FILE *fout; // pointer fisier output

typedef struct NodReconstruire {    // structura pentru arborele huffman ce va fi reconstruit din preordinea citita din fisierul binar input
    char ch;
    struct NodReconstruire *st, *dr;
    int children;
}NodReconstruire;

extern NodReconstruire* radacina_reconstr; // radacina arbore ce urmeaza sa fie reconstruit

extern char preordine[128]; // vector in care se va retine preordinea citita in forma (2) - mai jos
extern char preordine_caractere[128]; // vector  in care se vor retine doar caracterele din preordine (fara elementele '-', care reprezinta noduri neterminale)

extern int numar_noduri; // retine cate noduri avem de adaugat in arborele huffman ce va fi creat din informatia continuta in header

extern int caracter_de_adaugat;        // "boolean" 0/1 - flag ce este pus pe 1 in momentul in care in preordinea citita din header-ul fisierului
                                    // de intrare este citit un bit 1 ( => ca urmatorii 8 biti vor reprezenta valoarea unui caracter frunza in arbore);
                                    // arborele este stocat in header cu ajutorul preordinii lui, care e de forma "0001a1b01c,
                                    // 0 = nod neterminal, 1 = marcheaza ca urmatorii 8 biti vor reprezenta valoarea ASCII a unui nod frunza
extern int caractere_adaugate; // initializare contor caractere adaugate in arbore

extern int idx_preordine;  // contor caractere adaugate in preordine[], incluzand caracterele ce nu sunt noduri terminale in arborele huffman (in cazul prezent '-')
extern int idx_preordine_caractere;    // contor ce retine exclusiv nr de caractere noduri terminale in arborele huffman

extern int arr[100], varf; // folosite in functia huffman afisare coduri

extern unsigned char buffer; // buffer de 1 byte in care vom citi informatiile din fisier
extern int bits[8]; // byte buffer de 0 | 1 intregi

extern char *cod_buffer; // vector de char ce va fi alocat dinamic pentru a retine nr_max_biti returnat din functia de mai sus;
                  // va retine cate un bit citit din textul encoded pana cand codul retinut in cod_buffer este egal cu
                  // unul dintre codurile retinute in matricea de coduri huffman

extern int sfarsit;

NodReconstruire* NodReconstruire_nod_nou(char ch);

int Nod_este_frunza_dec(NodReconstruire* radacina);

void parcurgere_inordine_dec(NodReconstruire* radacina);

int ch_find_index_in_vector_preordine(char ch);

void cod_stocare_afisare_dec(int arr[], int n, char ch);

void HuffmanTree_afisare_coduri_dec(NodReconstruire* radacina, int arr[], int varf);

NodReconstruire* arbore_construire_preordine(char *sir, int *idx);

void unsigned_char_to_int(unsigned char c);

int binary_to_int(int *bits);

void bit_scriere_bit_dec(int bit);

int inaltime_arbore(NodReconstruire* radacina);

void huffman_decode(char *nume_fisier_intrare);

#endif // DECODE_H
