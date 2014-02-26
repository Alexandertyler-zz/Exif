#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <zlib.h>
#include "png.h"

long int chunk_length(char* buffer) {
  char *buffer2;
  //int *digits;
  //digits = (int *) malloc(4*sizeof(int));
  buffer2 = (char *) malloc(4*sizeof(char));
  int length = 0;
  /* Converts the hex in buffer to ints in char array */
  sprintf(buffer2, "%d%d%d%d", buffer[0], buffer[1], buffer[2], buffer[3]);
  /* Converts the ints in char array to int array */
  //printf("%ld\n", strtol(buffer, NULL, 0));
  //printf("%X\n", buffer);
  //sscanf(buffer2, "%d %d %d %d", &digits[0], &digits[1], &digits[2], &digits[3]);
  //printf("%d:%d:%d:%d\n", digits[0], digits[1], digits[2], digits[3]);
  //length = digits[0] + (digits[1] * 16) + (digits[2] * 256) + (digits[3] * 2096);
  length = atoi(buffer2);
  printf("atoi length: %d\n", length);
  //free(digits);
  free(buffer2);
  return length;
}


int analyze_chunks(FILE *f) {
  //char tEXt[4] = {0x74, 0x45, 0x58, 0x74};
  //char zTXt[4] = {0x7A, 0x54, 0x58, 0x74};
  //char tIME[4] = {0x74, 0x49, 0x4D, 0x45};
  /* Idea for proceeding: we need to go through chunks of code. Chunks take the form:
     length of data portion of chunk (4 bytes) chunktype (4bytes) data (variable bytes) checksum (4 bytes) 
     So process first 8 bytes of next chunk. If the last 4 bytes match chunktypes above, then process
     and output data portion (after verifying checksum). Otherwise skip ahead the length of the data 
     + 4 bytes. */
  /* This function only analyzes 8 bytes at a time */
  char length[4];
  //char chunktype[4];
  int chunklength;  
  while(fread(length, 1, 4, f) == 4) {
    int i = 0;
    chunklength = chunk_length(length);
    //chunklength = 0;
    printf("This chunk has length %d\n", chunklength);
    while ( i < 4 ) {
      printf("%X:",(unsigned char)length[i] );
      //printf("%x",tEXt[i-4]);
      //printf("%x",zTXt[i-4]);
      //printf("%x",tIME[i-4]);
      i++;
    }
    //if (strcmp(buffer+4, tEXt) == 0) {
    //  printf("We gots a tEXt.");
      /* atoi will take the first four characters and output an integer 
	 but apparently this doesnt work at all */
    //  fseek(f, chunklength, SEEK_CUR);
    //}
    //else if (strcmp(buffer+4, zTXt) == 0) {
    //  printf("We gots a zTXt. Decompress that shit.");
    //  fseek(f, chunklength, SEEK_CUR);
    //}
    //else if (strcmp(buffer+4, tIME) == 0) {
    //  printf("What tIME is it? PARTY TIME!");
    //  fseek(f, chunklength, SEEK_CUR);
    //} else {   
      //getchar();      
      //printf("Who cares? Moving ahead %d spaces\n", moveahead);
      //fseek(f,chunklength, SEEK_CUR);
      //}
    //getchar();
  }
  if (feof(f)) {
    printf("EOF reached.");
    return 0;
  }
  else {
    printf("error");
    return -1;
  }
}
 
/*
 * Analyze a PNG file.
 * If it is a PNG file, print out all relevant metadata and return 0.
 * If it isn't a PNG file, return -1 and print nothing.
 */
int analyze_png(FILE *f) {
  /* YOU WRITE THIS PART */
  char png_header[8] = {0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a};
  /* Read in first 8 bytes and compare with standard PNG intro to determine if PNG file */
  char buffer[8];
  /* Reads into buffer, each element 1 byte, read 8 bytes, read from input stream f */
  fread(buffer, 1, 8, f);
  //printf("%s", png_header);
  //getchar();
  //printf("This number: %ld\n", length);
  //This snippet could be useful for future reference when trying to examine hex 
  /*int i = 0;
  while (i < 8 ) {
    printf("%x\n", buffer[i]);
    printf("%x\n", png_header[i]);
    i++;
  }
  getchar();*/
  if (strcmp(buffer,png_header) == 0) {
    analyze_chunks(f);
    printf("THIS SHITS A PNG!\n");
    return 1;
  } else {
    printf("Not even a little bit of a PNG.\n");
    return -1;
  }
}

 
