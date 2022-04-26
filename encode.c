#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "encode.h"

unsigned char bit_buffer_enc;       // buffer pe 1 byte ce ajuta in scrierea bitilor in fisierul out pt ca rezolutia in prelucratul fisierelor in C este de 1 byte
int bit_nr_biti_retinuti_enc = 0;   // contor care numara cati biti sunt retinuti in buffer-ul de 8 biti

char **coduri_huffman_matrice; // matrice in care se vor stoca codurile huffman ale caracterelor, va fi alocata dinamic

FILE *fin;  // pointer fisier input
FILE *fout; // pointer fisier output

Element_Frecventa *vector_frecventa;    // vectorul de frecventa, va fi alocat dinamic

char *sir; // sir ce urmeaza sa fie citit
int nr_caractere = 0;   // initializare contor caractere distincte din sir (va fi folosit in prelucrarea vectorului de frecventa)
int lungime_sir = 0;    // initializare contor nr_caractere al sirului ce urmeaza a fi citit

unsigned int Tree_nr_caractere; // retine numarul de caractere distincte ce se afla in arborele huffman creat
                                // (e reprezentat pe 4 bytes dar vor fi folositi doar primii 8LSB pentru ca in ASCII avem 128 caractere

Nod* radacina;  // radacina arborelui huffman ce va fi creat

int Nod_este_frunza_enc(Nod* radacina) { // functie care verifica daca nodul dat ca parametru este frunza

    return !(radacina -> st) && !(radacina -> dr);
}

Nod* Nod_creare(char ch, int frecv) { // functie care aloca spatiu unui nod nou cu un caracter si o frecventa date
    struct Nod* temp = (struct Nod*)malloc(sizeof(struct Nod));

    temp -> st = temp -> dr = NULL;
    temp -> ch = ch;
    temp -> frecv = frecv;

    if(ch != '-') Tree_nr_caractere++;  // daca valoarea caracterului nu este '-' => avem un caracter nou adaugat in arbore
                                        // => incrementam contorul de caractere Tree_nr_caractere

    return temp;
}

Tree* Tree_creare(unsigned capacitate) { // functie ce aloca spatiu unui arbore de o capacitate data

    Tree* tree = (Tree*)malloc(sizeof(Tree));

    tree->nr_noduri = 0;

    tree->capacitate = capacitate;

    tree->arr = (Nod**)malloc(tree -> capacitate * sizeof(Nod*));

    return tree;
}

void Nod_interschimbare_noduri(Nod** a, Nod** b) { // functie ce interschimba 2 noduri ale unui arbore

    Nod* t = *a;
    *a = *b;
    *b = t;

}

void Tree_minHeapify(Tree* tree, int idx) { //functie pt min heapify (rearanjare min heap)

    int minim = idx;
    int st = 2 * idx + 1;
    int dr = 2 * idx + 2;

    if (st < tree -> nr_noduri && tree -> arr[st] -> frecv < tree -> arr[minim]->frecv)
        minim = st;

    if (dr < tree -> nr_noduri && tree -> arr[dr] -> frecv < tree -> arr[minim]->frecv)
        minim = dr;

    if (minim != idx) {
        Nod_interschimbare_noduri(&tree -> arr[minim], &tree -> arr[idx]);
        Tree_minHeapify(tree, minim);
    }
}

int Tree_un_nod(Tree* tree) { // returneaza true daca arborele dat ca parametru are doar 1 nod
    return (tree -> nr_noduri == 1);
}

Nod* Tree_extragere_nod_minim(Tree* tree) { // functie ce extrage minimul din minHeap

    Nod* temp = tree->arr[0];
    tree->arr[0] = tree->arr[tree->nr_noduri - 1];

    --tree->nr_noduri;
    Tree_minHeapify(tree, 0); // am sters / extras radacina si am inlocuit-o cu ultimul element din arbore, deci trebuie sa rearanjam minHeap-ul

    return temp;
}

void Tree_inserare(Tree* tree, Nod* nod) { // functie pentru introducerea unui nod in minHeap

    ++tree -> nr_noduri;
    int i = tree -> nr_noduri - 1;

    while (i && nod -> frecv < tree -> arr[(i - 1) / 2] -> frecv) {
        tree -> arr[i] = tree -> arr[(i - 1) / 2];
        i = (i - 1) / 2;
    }

    tree -> arr[i] = nod;
}

void Tree_construire_minHeap(Tree* tree) { // functie construire minHeap

    int n = tree -> nr_noduri - 1;
    int i;

    for (i = (n - 1) / 2; i >= 0; --i)
        Tree_minHeapify(tree, i);
}

int ch_find_index_in_vector_frecventa(char ch) { // functie care returneaza index-ul caracterului dat ca parametru in vector_frecventa
    for(int i=0; i < nr_caractere; i++) {
        if(ch == vector_frecventa[i].ch) return i;
    }
    return -1;
}

void cod_stocare_afisare_enc(int arr[], int n, char ch) { // functie pt stocarea si afisarea unui cod de n caractere
    coduri_huffman_matrice = (char**) realloc ( coduri_huffman_matrice, nr_caractere * sizeof(char*) ); // alocare dinamica "linii" in "matrice"

    coduri_huffman_matrice[ ch_find_index_in_vector_frecventa(ch) ] = (char*) malloc ( (n + 1)* sizeof(char) ); //alocare dinamica "coloane" in "matrice"

    int i;
    for (i = 0; i < n; i++){
        coduri_huffman_matrice[ch_find_index_in_vector_frecventa(ch)][i] = arr[i] + '0'; // " + '0' " <=> transformare arr[i] din intreg in char
    }

    coduri_huffman_matrice[ch_find_index_in_vector_frecventa(ch)][i] = '\0';

    printf("%s", coduri_huffman_matrice[ch_find_index_in_vector_frecventa(ch)]);

    printf("\n");
}

Tree* Tree_creare_construire_minHeap(Element_Frecventa vect_frecv[], int nr_noduri) { // functie ce creeaza un minHeap de capacitate egala cu nr_noduri si insereaza toate caracterele din vectorul date[] in arbore

    Tree* minHeap = Tree_creare(nr_noduri);

    for (int i = 0; i < nr_noduri; ++i)
        minHeap -> arr[i] = Nod_creare(vect_frecv[i].ch, vect_frecv[i].frecv);

    minHeap -> nr_noduri = nr_noduri;
    Tree_construire_minHeap(minHeap);

    return minHeap;
}

struct Nod* Tree_construire_HuffmanTree(Element_Frecventa vect_frecv[], int nr_noduri) { // functie ce construieste arborele huffman

    Nod *st, *dr, *varf;

    Tree* minHeap = Tree_creare_construire_minHeap(vect_frecv, nr_noduri); // in primul rand, se creeaza un minHeap de capacitate egala cu nr_noduri

    while (!Tree_un_nod(minHeap)) { // parcurgere minHeap atat timp cat minHeap-ul contine mai mult de 1 nod

        st = Tree_extragere_nod_minim(minHeap); //se extrag cele 2 elemente cu frecventa minima din Min Heap
        dr = Tree_extragere_nod_minim(minHeap);

        varf = Nod_creare('-', st -> frecv + dr -> frecv);  // apoi, se creeaza un nod nou cu frecventa = cu suma frecventelor celor 2 noduri extrase anterior (care sunt copiii stang, respectiv drept al nodului nou
                                                            // creat); caracterul '-' : folosit pt nodurile interne, care nu contin caractere propriu zise, doar suma de frecvente
        varf -> st = st;
        varf -> dr = dr;

        Tree_inserare(minHeap, varf); // se adauga nodul nou creat in Min Heap
    }

    return Tree_extragere_nod_minim(minHeap); // nodul ramas este radacina arborelui huffman
}

int HuffmanTree_inaltime(Nod *radacina) {   // functie ce returneaza inaltimea arboelui huffman <=> lungimea celui mai lung cod
    int h_st = 0, h_dr = 0;

    if(radacina == NULL) return 0;

    if(radacina -> st != NULL) {
        h_st = HuffmanTree_inaltime(radacina -> st);
    }
    if(radacina -> dr != NULL) {
        h_dr = HuffmanTree_inaltime(radacina -> dr);
    }

    return (h_st > h_dr)? h_st + 1 : h_dr + 1;
}

void HuffmanTree_afisare_coduri_enc(Nod* radacina, int arr[], int varf) { // functie ce printeaza codurile huffman de la radacina - create cu ajutorul arborelui huffman;
                                                                         // foloseste vectorul arr[] pt stocarea codurilor

    if (radacina -> st) { // asignare 0 pe partea stanga
        arr[varf] = 0;
        HuffmanTree_afisare_coduri_enc(radacina -> st, arr, varf + 1);
    }

    if (radacina -> dr) { //asignare 1 pe partea dreapta

        arr[varf] = 1;
        HuffmanTree_afisare_coduri_enc(radacina -> dr, arr, varf + 1);
    }

    if (Nod_este_frunza_enc(radacina)) { // daca nodul radacina dat este frunza => contine un caracter => afisare caracter + cod stocat in arr[]
        printf("%c: ", radacina -> ch);
        cod_stocare_afisare_enc(arr, varf, radacina -> ch);
    }
}

void HuffmanCodes(Element_Frecventa vect_frecv[], int nr_noduri) { // functie care creeaza arborele huffman si afiseaza codurile formate cu ajutorul lui

    int *arr, varf = 0;

    radacina = Tree_construire_HuffmanTree(vect_frecv, nr_noduri); // construire arbore huffman

    arr = (int*) malloc ( HuffmanTree_inaltime(radacina) * sizeof(int) );   // *arr e folosit pt stocarea codurilor => se aloca dinamic un vector
                                                                                    // de marimea celui  mai lung cod huffman == inaltimea arborelui huffman

    HuffmanTree_afisare_coduri_enc(radacina, arr, varf);
}

void Element_Frecventa_interschimbare(Element_Frecventa *x, Element_Frecventa *y) { // functie ce interschimba 2 elemente din vectorul de frecventa; va fi folosita la selection sort
    Element_Frecventa temp = *x;
    *x = *y;
    *y = temp;
}

void Element_Frecventa_SelectionSort(Element_Frecventa arr[], int n) // functie sortare selection sort
{
    int i, j, min_idx;

    for (i = 0; i < n-1; i++)
    {
        min_idx = i;        // se gaseste elementul minim in vectorul nesortat
        for (j = i+1; j < n; j++)
          if (arr[j].frecv < arr[min_idx].frecv)
            min_idx = j;

        Element_Frecventa_interschimbare(&arr[min_idx], &arr[i]); // se interschimba minimul gasit cu primul element
    }
}

void cod_sir_afisare(char *sir) { // functie ce afiseaza fragmentul dat codificat
    for(int i=0; i < lungime_sir; i++) {
        printf("%s", coduri_huffman_matrice[ch_find_index_in_vector_frecventa(sir[i])]);
    }
}

int nr_biti_codare_ascii(char *sir) {   // functie ce returneaza nr-ul de biti de care e nevoie pt a stoca caracterele sub forma ASCII ale unui sir intr-un fisier
    return strlen(sir) * 8;  // fiecare char ocupa 8 biti
}

int nr_biti_codare_huffman() {   // functie ce returneaza nr-ul de biti de care e nevoie pt a stoca caracterele sub forma Huffman ale unui sir intr-un fisier
    int total_biti = 0;          // initializare variabila in care va fi stocat rezultatul
    printf("\n");
    for(int i=0; i < nr_caractere; i++) {
        total_biti += vector_frecventa[i].frecv * strlen( coduri_huffman_matrice[ch_find_index_in_vector_frecventa(vector_frecventa[i].ch)] );
        printf("%d * %d ", vector_frecventa[i].frecv, strlen( coduri_huffman_matrice[ch_find_index_in_vector_frecventa(vector_frecventa[i].ch)]) );
        if( i != nr_caractere-1 ) printf("+ ");
    }
    printf("\n= ");

    return total_biti;
}

void bit_scriere_bit_enc(int bit) { // functie ce scrie bit-ul 0/1 dat ca parametru in buffer-ul bit_buffer pe 1 byte
    bit_buffer_enc <<= 1;           // shiftare la stanga bit buffer pentru a face loc bitului nou
    if (bit) bit_buffer_enc |= 0x1;   // daca bitul de scris este 1, setam LSB-ul bit bufferului pe 1

    bit_nr_biti_retinuti_enc++;        // incrementare contor biti in buffer

    if (bit_nr_biti_retinuti_enc == 8) // daca byte-ul buffer este ocupat complet (s-au scris 8 biti), se scrie byte-ul in fisier out
    {
        fwrite(&bit_buffer_enc, 1, sizeof(bit_buffer_enc), fout);
        bit_nr_biti_retinuti_enc = 0;
        bit_buffer_enc = 0;
    }
}

void bit_flush_biti()   // functie ce umple restul bit_buffer-ului cu biti de 0 pana cand numarul de biti retinuti / scrisi in bit_buffer este 8
{
  while (bit_nr_biti_retinuti_enc)
    bit_scriere_bit_enc(0);
}

void ch_print_char_to_fout(char c) {    // functie ce ia paramterul char c si ii scrie fiecare bit in bit_buffer pana cand se ocupa complet si este scris in fisierul output
    for (int i = 0; i < 8; i++) {
      if( !!((c << i) & 0x80) == 0 ) bit_scriere_bit_enc(0);
      else bit_scriere_bit_enc(1);
  }
}

void intreg_print_unsigned_int_to_fout(unsigned int intreg) {   // functie ce ia paramterul unsigned int intreg si ii scrie fiecare bit in bit_buffer pana cand se ocupa complet si este scris in fisierul output
    for (int i = 0; i < 8; i++) {
      if( !!((intreg << i) & 0x80) == 0 ) bit_scriere_bit_enc(0);
      else bit_scriere_bit_enc(1);
  }
}

void Tree_parcurgere_preordine_scriere_fout(Nod* radacina) { // functie ce parcurge arborele huffman in preordine, si scrie
                                                             // sirul de biti preordine astfel obtinut in fisierul output;
                                                             // 0 = nod neterminal '-';
                                                             // 1caracter = 1 - semnaleaza ca urmeaza un nod frunza, caracter = valoarea ASCII a caracterului retinut in nodul frunza

    if (radacina == NULL) {
        return;
    }

    printf("%c ", radacina->ch);
    if (radacina -> ch == '-' ) {   //daca nodul curent din arbore este '-', inseamna ca nu este un nod frunza, ce contine un caracter => scriem 0.
            bit_scriere_bit_enc(0);
    }
    else {  //altfel, avem nod frunza => scriem in fisier 1 urmat de caracterul respectiv e.g. pt C: "1C"; 1 - pt a semnala ca urmeaza un nod frunza, caracterul pt a sti ce valoare are nodul
        bit_scriere_bit_enc(1);
        ch_print_char_to_fout(radacina -> ch);
    }
    Tree_parcurgere_preordine_scriere_fout(radacina->st);
    Tree_parcurgere_preordine_scriere_fout(radacina->dr);
}

void cod_sir_print_fout(char *sir) { // functie ce scrie in fisierul output fragmentul dat codificat
    for(int i=0; i < lungime_sir; i++) {
        for(int j=0; j < strlen(coduri_huffman_matrice[ch_find_index_in_vector_frecventa(sir[i])]); j++ ) {
             if(coduri_huffman_matrice[ch_find_index_in_vector_frecventa(sir[i])][j] == '0') bit_scriere_bit_enc(0);
             else bit_scriere_bit_enc(1);
        }
    }
}

void parcurgere_inordine_enc(Nod* radacina) {   // functie de parcurgere arbore in inordine pt a verifica ca am creat arborele corect
    if (radacina == NULL) {
        return;
    }

    parcurgere_inordine_enc(radacina->st);
    printf("%c ", radacina->ch);
    parcurgere_inordine_enc(radacina->dr);
}


void huffman_encode(char *nume_fisier_intrare) {
    char ch;    // buffer in care se citeste cate 1 caracter din fisierul text

    char *fisier_out_nume;     // stocheaza numele fisierului de scris ( fisier_in_nume + ".huffman" ) output

    fisier_out_nume = (char*) malloc ( (strlen(nume_fisier_intrare) + 12) * sizeof(char) );     // +11 = 11 caractere suplimentare in extensia .compressed, +1 = caracterul terminal '\0'

    fin = fopen(nume_fisier_intrare, "r");

    strcpy(fisier_out_nume, nume_fisier_intrare);
    strcat(fisier_out_nume, ".compressed");

    fout = fopen(fisier_out_nume, "wb");

    if (fin == NULL || fout == NULL)
    {
        printf("Eroare deschidere fisiere.\n");
    }

    while ( (ch = fgetc(fin)) != EOF ) { // citire caracter cu caracter din fisierul text input

        sir = (char*)realloc(sir, (lungime_sir+1) * sizeof(char));
        sir[lungime_sir++] = ch;

        int i=0;

        do{
            if( nr_caractere == 0 ) {   // nu exista deloc caractere adaugate in vector_frecventa => caracterul trebuie adaugat in vector_frecventa
                vector_frecventa = (Element_Frecventa*) realloc ( vector_frecventa, (nr_caractere + 1) * sizeof(Element_Frecventa));
                vector_frecventa[nr_caractere].ch = ch;
                vector_frecventa[nr_caractere++].frecv = 1;
                break;
            }
            else {
                    if (ch == vector_frecventa[i].ch) { // caracterul exista deja in vector_frecventa <=> nu e prima data cand e intalnit in text, se incrementeaza doar frecventa
                        vector_frecventa[i].frecv ++;
                        break;
                    }

                    else if(i == nr_caractere - 1) {    // caracterul nu a fost gasit in vector_frecventa <=> este prima aparitie a lui in text => trebuie adaugat
                        vector_frecventa = (Element_Frecventa*) realloc ( vector_frecventa, (nr_caractere + 1) * sizeof(Element_Frecventa));
                        vector_frecventa[nr_caractere].ch = ch;
                        vector_frecventa[nr_caractere ++].frecv = 1;
                        break;
                    }
            }
            i++;

        } while(i < nr_caractere);
    }

    vector_frecventa = (Element_Frecventa*) realloc ( vector_frecventa, (nr_caractere + 1) * sizeof(Element_Frecventa));
    vector_frecventa[nr_caractere].ch = '&'; // pseudo-EOF de adaugat la sfarsitul textului encoded pentru a sti programul cat sa continue procesul de decoding
    vector_frecventa[nr_caractere ++].frecv = 0;

    sir = (char*)realloc(sir, (lungime_sir+1) * sizeof(char));
    sir[lungime_sir++] = '&';
    sir[lungime_sir] = '\0';

    Element_Frecventa_SelectionSort(vector_frecventa, nr_caractere); // sortare vector_frecventa dupa frecventa

    printf("CODURI:\n");
    HuffmanCodes(vector_frecventa, nr_caractere);   // afisare coduri huffman pt fiecare caracter

    printf("\n\nINORDINE ARBORE HUFFMAN CREAT:\n");
    parcurgere_inordine_enc(radacina);

    printf("\n\n\nTEXT ORIGINAL:\n%s", sir);
    printf("\n\n\nTEXT ENCODED:\n");
    cod_sir_afisare(sir);       // afisare sir codat huffman
    printf("\n\n\nNR. BITI CODARE ASCII: %d", nr_biti_codare_ascii(sir));
    printf("\n\n\nNR. BITI CODARE HUFFMAN: ");
    printf("%d", nr_biti_codare_huffman());

    printf("\n\n\n");
    intreg_print_unsigned_int_to_fout(Tree_nr_caractere);   // scriere in fisier fout nr de caractere ce se afla in arborele huffman

    printf("PREORDINE ARBORE HUFFMAN CREAT: \n");
    Tree_parcurgere_preordine_scriere_fout(radacina);       // scriere in fisier fout si afisare preordine arbore huffman creat
                                                            //pentru stocarea lui si pentru a se putea reproduce la decompresie
    bit_flush_biti();

    printf("\n\n\nNR CARACTERE IN ARBORE HUFFMAN: %d\n", Tree_nr_caractere);

    cod_sir_print_fout(sir);
    bit_flush_biti();

    free(vector_frecventa);                 //eliberare memorie alocata dinamic & inchidere fisiere
    free(sir);
    for(int i=0; i<nr_caractere; i++) {
        free(coduri_huffman_matrice[i]);
    }
    free(coduri_huffman_matrice);
    fclose(fin);
    fclose(fout);
    free(fisier_out_nume);
}
