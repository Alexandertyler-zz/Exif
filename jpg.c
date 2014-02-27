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
  return;

}

void analyze_jpgchunks(FILE *f) {
  //Marker is 2 bytes
  //length is 2 bytes
  //data is (length-2) bytes
  char chunk_front[2] = {0xff};
  //char super_chunk[10] = {0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5,
  //                          0xd6, 0xd7, 0xd8};
  char super_chunk_end[2] = {0xd9};
  char APP1[2] = {0xe1};

  char *buffer;
  buffer = malloc(1*sizeof(char));
  while(fread(buffer, 1, 1, f) == 1) {
    if (strcmp(buffer, chunk_front) == 0) {
      printf("Matched front of chunk\n");
      fread(buffer, 1, 1, f);
      if (strcmp(buffer, APP1) == 0) {
        printf("APP1 found\n");
        analyze_tiff(f);
      } else if (strcmp(buffer, super_chunk_end) == 0) {
        printf("End of file\n");
        return;
      } else {
        printf("No chunks yet.\n");
      }
      getchar();
    } else {
      printf("No chunk front match.\n");
    }
  }
}

int analyze_jpg(FILE *f) {
  // YOU WRITE THIS PART 
  char jpg_SOI[3] = {0xff, 0xd8};
  //char jpg_EOI[2] = {0xff, 0xd9};
  char *buffer;
  buffer = malloc(2*sizeof(char));
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
    return 1;
  } else {
    printf("We've lost contact with the jpg sir.\n");
    return -1;
  } 
}
