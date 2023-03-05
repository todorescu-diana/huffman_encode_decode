# huffman_encode_decode
Huffman compression & decompression program;  

# Program arguments  
After compiling, the program receives 3 arguments under the form of arg1 arg2 arg3, where:  
  - arg1 = executable file path  
  - arg2 = enc / dec, where:  
        enc = encoding (compression),  
        dec = decoding (decompression)      
  - arg3 = name of the file to be compressed / decompressed  

# Compression
In the case of compression function calling, the program will create a new file, which results out of the initial file after it is compressed, with the name initialFileName + ".compressed".  

 - The text of the input file is read
 - A frequency array is formed out of character occurrences in the text
 - A minHeap is created with the help of the frequency array
 - The Huffman Tree is created out of the minHeap
 - Character codes are formed by traversing the Huffman Tree
 - In the out file, the following items are stored:
              -- the number of characters stored in the Huffman Tree
              -- the preorder of the Huffman Tree;
                      the preorder is stored in the form (example): 0001a1b01c, where:
                          0 = non-leaf node
                          1 = signals that the following 8 bits will store the ASCII code of a leaf node
                          a, b, c = 8 bits ASCII code, leaf nodes in the tree
              -- the encoded text
                    
 Because the smallest piece of data one can write in a file in C is 1 byte, when the writing in the out file of the bits of the encoded text is done, the rest of the bits until filling the last byte will be 0, so we use a pseudo-EOF, the character '&', that lets us know when the encoded text finishes, and the 0 bits that we do not need start.   

# Decompression  
In the case of decompression function calling, the program will create a new file, which results out of the initial file after it is decompressed, with the name initialFileName + ".decompressed".  

- Program reads 1 byte which represents the number of characters that the reconstructed tree will contain
- The preorder of the original Huffman Tree is read & saved; out of the initial preorder in the form 0001a1b01c, a new preorder is formed, in the form ---ab-c, in which the '-' nodes are non-leaf nodes, and the nodes 'a', 'b', 'c' are leaf nodes
- The tree is reconstructed using the new formed preorder
- The character codes are formed traversing the reconstructed tree
- The encoded text is read and decoded; when a character code is found, it is written in the out file
- The reading process halts when encountering the code of the pseudo-EOF, '&'.

# Display output
Besides creating the files, other info is printed, such as the resulting Huffman codes for the characters, the preorder & inorder of the constructed tree (in the case of compression), or of the reconstructed tree (in the case of decompression), the encoded / decoded text; the program also displays the number of bits necessary for storing the text in both ASCII coding, as well in Huffman coding, in case of compression.

# Example
The file "example.txt" can be used for compresson, and then the file with the extension ".compressed" which results: for decompression.  
  
enc example.txt => example.txt.compressed  
dec example.txt.compressed => example.txt.compressed.decompressed  
