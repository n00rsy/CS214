tokenize: tokenize.o arraylist.o huffman.o
	gcc -std=gnu99 tokenize.o arraylist.o huffman.o -o tokenize
tokenize.o: tokenize.c
	gcc -c -std=gnu99 tokenize.c 
arraylist.o: arraylist.c include/arraylist.h
	gcc -c -std=gnu99 arraylist.c	
huffman.o: huffman.c include/huffman.h
	gcc -c -std=gnu99 huffman.c	
clean:
	rm *.o

