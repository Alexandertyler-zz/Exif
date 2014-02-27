#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "jpg.h"

/*
 * Analyze a JPG file that contains Exif data.
 * If it is a JPG file, print out all relevant metadata and return 0.
 * If it isn't a JPG file, return -1 and print nothing.
 */
void analyze_tiff(FILE *f) {
  printf("Inside analyze_tiff");
  char *buffer;
  buffer = malloc(2*sizeof(char));
  //this is endianness, 2 bytes
  fread(buffer, 1, 2, f);
  
  free(buffer)
  return;

}

long int standardLength(unsigned char* buffer) {
  unsigned char* buffer2;
  buffer2 = (char*) malloc(2*sizeof(unsigned char));
  sprintf(buffer2, "%02x%02X", buffer[0], buffer[1]);
  unsigned char *end;
  long int lint0 = strtol(buffer2, &end, 16);
  free(buffer2);
  return lint0;
} 

void analyze_jpgchunks(FILE *f) {
  //Marker is 2 bytes
  //length is 2 bytes
  //data is (length-2) bytes
  unsigned char chunk_front[2] = {0xff};
  
  unsigned char superCd0[2] = {0xd0};
  unsigned char superCd1[2] = {0xd1};
  unsigned char superCd2[2] = {0xd2};
  unsigned char superCd3[2] = {0xd3};
  unsigned char superCd4[2] = {0xd4};
  unsigned char superCd5[2] = {0xd5};
  unsigned char superCd6[2] = {0xd6};
  unsigned char superCd7[2] = {0xd7};
  unsigned char superCd8[2] = {0xd8};
  unsigned char superCda[2] = {0xda};

  unsigned char super_chunk_end[2] = {0xd9};
  unsigned char APP1[2] = {0xe1};

  unsigned char *buffer;
  buffer = malloc(1*sizeof(unsigned char));
  while(fread(buffer, 1, 1, f) == 1) {
    if (strcmp(buffer, chunk_front) == 0) {
      printf("Matched front of chunk\n");
      fread(buffer, 1, 1, f);
      if (strcmp(buffer, APP1) == 0) {
        printf("APP1 found\n");
        analyze_tiff(f);
      } else if (!strcmp(buffer,superCd0) || !strcmp(buffer,superCd1) || !strcmp(buffer,superCd2)
                  || !strcmp(buffer,superCd3) || !strcmp(buffer,superCd4) || !strcmp(buffer,superCd5)
                  || !strcmp(buffer,superCd6) || !strcmp(buffer,superCd7) || !strcmp(buffer,superCd8)
                  || !strcmp(buffer,superCda)) {
        printf("Superchunk engaged.\n");

      } else if (strcmp(buffer, super_chunk_end) == 0) {
        printf("End of file\n");
        return;
      } else {
        printf("Standard chunk. Finding length.\n");
        unsigned char *lengthBuffer;
        lengthBuffer = malloc(2*sizeof(unsigned char));
        fread(buffer, 1, 2, f);
        int chunkLength;
        chunkLength = standardLength(lengthBuffer);
        fseek(f, chunkLength-2, SEEK_CUR);
        free(lengthBuffer);
      }
      getchar();
    } else {
      printf("No chunk front match.\n");
    }
  }
  free(buffer);
}

int analyze_jpg(FILE *f) {
  // YOU WRITE THIS PART 
  unsigned char jpg_SOI[3] = {0xff, 0xd8};
  //char jpg_EOI[2] = {0xff, 0xd9};
  unsigned char *buffer;
  buffer = malloc(2*sizeof(unsigned char));
  fread(buffer, 1, 2, f);
  int i = 0;
  while (i < 2) {
    printf("buffer is: %x\n", buffer[i]);
    printf("jpg_SOI is: %x\n", jpg_SOI[i]);
    i++;
  }
  if (!strcmp(buffer,jpg_SOI)) {
    analyze_jpgchunks(f);
    printf("JPG Confirmed.\n");
    free(buffer);
    return 1;
  } else {
    printf("We've lost contact with the jpg sir.\n");
    free(buffer);
    return -1;
  } 
}
