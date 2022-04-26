#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "encode.h"
#include "decode.h"

int main(int argc, char **argv) {   // argv[0] = .exe path, argv[1] = "enc" / "dec", argv[2] = nume fisier
    if(argc > 1) {
        if(strcmp(argv[1], "enc") == 0) {
            printf("ENCODE\n\n");
            huffman_encode(argv[2]);
        }
        else{
            if(strcmp(argv[1], "dec") == 0) {
                printf("DECODE\n\n");
                huffman_decode(argv[2]);
            }
            else {
                printf("Argument invalid.\n");
            }
        }
    }
    else {
            printf("Argument invalid.\n");
    }
    return 0;
}
