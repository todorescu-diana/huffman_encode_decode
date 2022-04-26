#ifndef ENCODE_H
#define ENCODE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern unsigned char bit_buffer_enc;       // buffer pe 1 byte ce ajuta in scrierea bitilor in fisierul out pt ca rezolutia in prelucratul fisierelor in C este de 1 byte
extern int bit_nr_biti_retinuti_enc;   // contor care numara cati biti sunt retinuti in buffer-ul de 8 biti

extern char **coduri_huffman_matrice; // matrice in care se vor stoca codurile huffman ale caracterelor, va fi alocata dinamic

extern FILE *fin;  // pointer fisier input
extern FILE *fout; // pointer fisier output

typedef struct Nod {        // structura unui nod din arborele huffman
    unsigned frecv;         // de cate ori apare in text caracterul
    char ch;                // caracterul
	struct Nod *st, *dr;    // pointeri catre nodurile/copiii din stanga & dreapta
} Nod;

typedef struct Tree {       // structura unui arbore ( Huffman / minHeap )
    unsigned nr_noduri;     // numarul de noduri al arborelui
    unsigned capacitate;    // capacitatea arboelui (cate noduri "incap" in arbore)
    struct Nod** arr;    // vector de pointeri tip Nod
} Tree;

typedef struct Element_Frecventa {  // structura unui element retinut in vectorul de frecventa (care retine de cate ori apare fiecare litera in text
    char ch;                        // caracterul
    unsigned frecv;                 // frecventa
}Element_Frecventa;

extern Element_Frecventa *vector_frecventa;    // vectorul de frecventa, va fi alocat dinamic

extern char *sir; // sir ce urmeaza sa fie citit
extern int nr_caractere;   // initializare contor caractere distincte din sir (va fi folosit in prelucrarea vectorului de frecventa)
extern int lungime_sir;    // initializare contor nr_caractere al sirului ce urmeaza a fi citit

extern unsigned int Tree_nr_caractere; // retine numarul de caractere distincte ce se afla in arborele huffman creat
                                // (e reprezentat pe 4 bytes dar vor fi folositi doar primii 8LSB pentru ca in ASCII avem 128 caractere

extern Nod* radacina;  // radacina arborelui huffman ce va fi creat

int Nod_este_frunza_enc(Nod* radacina);

Nod* Nod_creare(char ch, int frecv);

Tree* Tree_creare(unsigned capacitate);

void Nod_interschimbare_noduri(Nod** a, Nod** b);

void Tree_minHeapify(Tree* tree, int idx);

int Tree_un_nod(Tree* tree);

Nod* Tree_extragere_nod_minim(Tree* tree);

void Tree_inserare(Tree* tree, Nod* nod);

void Tree_construire_minHeap(Tree* tree);

int ch_find_index_in_vector_frecventa(char ch);

void cod_stocare_afisare_enc(int arr[], int n, char ch);

Tree* Tree_creare_construire_minHeap(Element_Frecventa vect_frecv[], int nr_noduri);

struct Nod* Tree_construire_HuffmanTree(Element_Frecventa vect_frecv[], int nr_noduri);

int HuffmanTree_inaltime(Nod *radacina);

void HuffmanTree_afisare_coduri_enc(Nod* radacina, int arr[], int varf);

void HuffmanCodes(Element_Frecventa vect_frecv[], int nr_noduri);

void Element_Frecventa_interschimbare(Element_Frecventa *x, Element_Frecventa *y);

void Element_Frecventa_SelectionSort(Element_Frecventa arr[], int n);

void cod_sir_afisare(char *sir);

int nr_biti_codare_ascii(char *sir);

int nr_biti_codare_huffman();

void bit_scriere_bit_enc(int bit);

void bit_flush_biti();

void ch_print_char_to_fout(char c);

void intreg_print_unsigned_int_to_fout(unsigned int intreg);

void Tree_parcurgere_preordine_scriere_fout(Nod* radacina);

void cod_sir_print_fout(char *sir);

void parcurgere_inordine_enc(Nod* radacina);

void huffman_encode(char *nume_fisier_intrare);

#endif // ENCODE_H
