#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "jpg.h"

/*
 * Analyze a JPG file that contains Exif data.
 * If it is a JPG file, print out all relevant metadata and return 0.
 * If it isn't a JPG file, return -1 and print nothing.
 */
void analyze_chunks(FILE *f) {
  //Marker is 2 bytes
  //length is 2 bytes
  //data is (length-2) bytes
  char chunk_front[1] = {0xff};
  char super_chunk[9] = {0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5,
                            0xd6, 0xd7, 0xd8};
  char super_chunk_end[1] = {0xd9};
  char APP1[1] = {0xe1};

  char *buffer;
  buffer = malloc(1*sizeof(char));
  while(fread(buffer, 1, 1, f) == 1) {
    if (strcmp(buffer, chunk_front) == 0) {
      printf("Matched front of chunk\n");
      if (strcmp(buffer[1], APP1) == 0) {
        analyze_tiff(f);
      } else if (strcmp(buffer[1], super_chunk_end) == 0) {
        printf("End of file\n");
        return;
      } else {
        printf("No chunks yet.\n");
      }
    } else {
      printf("No chunk front match.\n");
    }
  }
}

int analyze_jpg(FILE *f) {
  /* YOU WRITE THIS PART */
  char jpg_SOI[2] = {0xff, 0xd8};
  //char jpg_EOI[2] = {0xff, 0xd9};
  char buffer[8];
  fread(buffer, 1, 2, f);

  if (strcmp(jpg_SOI, buffer) == 0) {
    analyze_chunks(f);
    printf("JPG Confirmed.\n");
    return 1;
  } else {
    printf("We've lost contact with the jpg sir.\n");
    return -1;
  }
}
