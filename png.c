#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <zlib.h>
#include "png.h"

long int chunk_length(char* buffer) {
  /*HOW IT WORKS: Take our initial buffer, store the values into a new buffer
  with leading 0s using the command sprintf. Then use strtol to convert
  our new string "buffer2" into int value using base 16."*/

  char *buffer2;
  //printf("Buffer is: %X %X %X %X\n", buffer[0], buffer[1], buffer[2], buffer[3]);
  buffer2 = (char *) malloc(4*sizeof(char));
  sprintf(buffer2, "%02X%02X%02X%02X",(unsigned char) buffer[0],(unsigned char) buffer[1],(unsigned char) buffer[2],(unsigned char) buffer[3]);
  //printf("Buffer2 is: %s\n", buffer2); 
  char *end;
  long int lint0 = strtol(buffer2, &end, 16);  
  //printf("Value is: %ld\n", lint0);
  free(buffer2);
  //getchar();
  return lint0;
}


int analyze_chunks(FILE *f) {
  char tEXt[4] = {0x74, 0x45, 0x58, 0x74};
  char zTXt[4] = {0x7a, 0x54, 0x58, 0x74};
  char tIME[4] = {0x74, 0x49, 0x4d, 0x45};
  /* Idea for proceeding: we need to go through chunks of code. Chunks take the form:
     length of data portion of chunk (4 bytes) chunktype (4bytes) data (variable bytes) checksum (4 bytes) 
     So process first 8 bytes of next chunk. If the last 4 bytes match chunktypes above, then process
     and output data portion (after verifying checksum). Otherwise skip ahead the length of the data 
     + 4 bytes. */
  /* This function only analyzes 8 bytes at a time */
  char *buffer;
  buffer = malloc(8*sizeof(char));
  //char chunktype[4];
  int chunklength;  
  while(fread(buffer, 1, 8, f) == 8) {
    //int i = 0;
    chunklength = chunk_length(buffer);
    //chunklength = 0;
    printf("This chunk has length %d\n", chunklength);
    int i = 0;
    while ( i < 8) {
      printf("%X:",(unsigned char) buffer[i] );
      //printf("%x",tEXt[i-4]);
      //printf("%x",zTXt[i-4]);
      //printf("%x",tIME[i-4]);
      i++;
    }
    printf("\n");
    //printf("buffer: %s matching_string: %s", buffer+4, tIME);
    
    if (strcmp(buffer+4, tEXt) == 0) {
      printf("We gots a tEXt.\n");
      /* atoi will take the first four characters and output an integer 
	 but apparently this doesnt work at all */
    //  fseek(f, chunklength, SEEK_CUR);
    }
    else if (strcmp(buffer+4, zTXt) == 0) {
      printf("We gots a zTXt. Decompress that shit.\n");
    //  fseek(f, chunklength, SEEK_CUR);
    }
    else if (strcmp(buffer+4, tIME) == 0) {
      printf("What tIME is it? PARTY TIME!\n");
    //  fseek(f, chunklength, SEEK_CUR);
    } else {   
      //getchar();      
      printf("Who cares?\n");
    }
    fseek(f,chunklength+4, SEEK_CUR);
    //getchar();
  }
  if (feof(f)) {
    printf("EOF reached.");
    free(buffer);
    return 0;
  }
  else {
    printf("error");
    free(buffer);
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

 
