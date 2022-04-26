# huffman_encode_decode
Fisiere pentru program compresie & decompresie Huffman;  

# Argumente rulare executabil  
Dupa compilare, programul primeste 3 argumente sub forma arg1 arg2 arg3, unde:  
  - arg1 = path-ul executabilului  
  - arg2 = enc / dec, unde:  
        enc = encoding (compresie),  
        dec = decoding (decompresie)      
  - arg3 = numele fisierului de comprimat / decomprimat  

# Compresie
In cazul apelarii functiei de compresie, programul va crea un fisier nou, ce rezulta din fisierul initial dupa ce este comprimat,  
cu numele numeFisierInitial + ".compressed".  

 - Se citeste textul fisierului dat
 - Se formeaza vector de frecventa din aparitiile caracterelor in text
 - Cu ajutorul vectorului de frecventa se creeaza un minHeap
 - Din minHeap se creeaza arborele Huffman
 - Prin parcurgerea arborelui Huffman se formeaza codurile caracterelor
 - In fisierul out sunt retinute:  
              -- numarul de caractere retinute in arborele Huffman  
              -- preordinea arborelui Huffman;  
                      preordinea este retinuta in forma (exemplu): 0001a1b01c, unde:     
                          0 = nod neterminal  
                          1 = semnaleaza ca urmatorii 8 biti vor retine codul ASCII al unui nod terminal  
                          a, b, c = 8 biti cod ASCII, noduri terminale in arbore  
                    
 Pentru ca rezolutia fisierelor C este de 1 byte, cand am terminat de scris bitii textului encoded in fisierul out, restul bitilor pana la umplerea ultimului  
 byte vor fi 0, deci se foloseste un pseudo-EOF, caracterul '&', pentru a sti cand se termina textul encoded si incep bitii 0 de flushed care nu trebuie luati  
 in considerare.   

# Decompresie  
In cazul apelarii functiei de decompresie, programul va crea un fisier nou, ce rezulta din fisierul initial dupa ce este decomprimat,
cu numele numeFisierInitial + ".decompressed".  

- Se citeste 1 byte reprezentand numarul de caractere ce il va contine arborele recreat  
- Se citeste si se salveaza preordinea arborelui Huffman original  
- Se reconstruieste arborele pornind de la preordine  
- Se formeaza codurile caracterelor parcurgand arborele reconstruit  
- Se citeste textul encoded si se decodifica; cand se gaseste un cod al unui caracter, se scrie in fisier out  
- Citirea se opreste la intalnirea codului pseudo-EOF -ului, '&'.  

# Afisare  
In afara de crearea fisierelor, se printeaza si informatii cum ar fi codurile Huffman create corespunzatoare caracterelor, preordinea & inordinea   
arborelui creat (in cazul compresiei), respectiv recreat (in cazul decompresiei), textul encoded / decoded; se mai afiseaza si numarul de biti necesar  
pentru fisier atat in codare ASCII, cat si in Huffman, in cazul compresiei.  
