#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "jpg.h"

/*
 * Analyze a JPG file that contains Exif data.
 * If it is a JPG file, print out all relevant metadata and return 0.
 * If it isn't a JPG file, return -1 and print nothing.
 */
long int tagLength(unsigned char* buffer) {
  unsigned char* buffer2;
  buffer2 = calloc(4, sizeof(unsigned char));
  sprintf((char *) buffer2, "%02x%02X%02X%02X", buffer[3], buffer[2], buffer[1], buffer[0]);
  char *end;
  long int lint0 = strtol((char *) buffer2, &end, 16);
  free(buffer2);
  //printf("Length is: %li\n", lint0);
  return lint0;
} 

void analyze_tag(FILE *f) {
  char DocName[3] = {0x01, 0x0d};
  char ImgDesc[3] = {0x01, 0x0e};
  char Make[3] = {0x01, 0x0f};
  char Model[3] = {0x01, 0x10};
  char Software[3] = {0x01, 0x31};
  char DateTime[3] = {0x01, 0x32};
  char Artist[3] = {0x01, 0x3b};
  char HostPC[3] = {0x01, 0x3c};
  char Copyright[3] = {0x82, 0x98};
  char RSF[3] = {0xa0, 0x04};
  char DTOriginal = {0x90, 0x03};
  char DTDigitized = {0x90, 0x04};
  char MakerNote = {0x92, 0x7c};
  char UserComment = {0x92, 0x86};
  char ImageUniqueID = {0xa4, 0x20};

  char ExifIFD = {0x87, 0x69};

 


  unsigned char* tagbuff;
  unsigned char* lenbuff;
  int length;
  tagbuff = calloc(4, sizeof(unsigned char));
  lenbuff = calloc(4, sizeof(unsigned char));
  fread(tagbuff, 1, 4, f);
  fread(lenbuff, 1, 4, f);
  length = (int) tagLength(lenbuff);
  if (!strncmp((char *)tagbuff,ExifIFD,2)) {

  } else if (!strncmp((char *)tagbuff,DocName,2)) {
    
  } else if (!strncmp((char *)tagbuff,ImgDesc,2)) {

  } else if (!strncmp((char *)tagbuff,Make,2)) {

  } else if (!strncmp((char *)tagbuff,Model,2)) {
  } else if (!strncmp((char *)tagbuff,Software,2)) {
  } else if (!strncmp((char *)tagbuff,DateTime,2)) {
  } else if (!strncmp((char *)tagbuff,Artist,2)) {
  } else if (!strncmp((char *)tagbuff,HostPC,2)) {
  } else if (!strncmp((char *)tagbuff,Copyright,2)) {
  } else if (!strncmp((char *)tagbuff,RSF,2)) {
  } else if (!strncmp((char *)tagbuff,DTOriginal,2)) {
  } else if (!strncmp((char *)tagbuff,DTDigitized,2)) {
  } else if (!strncmp((char *)tagbuff,MakerNote,2)) {
  } else if (!strncmp((char *)tagbuff,UserComment,2)) {
  } else if (!strncmp((char *)tagbuff,ImageUniqueID,2)) {
  } else {
  }
}


void analyze_IFD(FILE *f) {
  unsigned char* ifdbuff;
  unsigned char* lenbuff;
  ifdbuff = calloc(2, sizeof(unsigned char));
  fread(ifdbuff, 1, 2, f);
  lenbuff = calloc(2, sizeof(unsigned char));
  printf("ifdbuff is: %02X%02X\n", ifdbuff[0], ifdbuff[1]);
  sprintf((char *) lenbuff, "%02x%02X", ifdbuff[1], ifdbuff[0]);
  char *end;
  long int lint0 = strtol((char *) lenbuff, &end, 16);
  free(lenbuff);
  free(ifdbuff);
  printf("Number of tags is: %li\n", lint0);
  while (lint0 >= 0) {
    printf("Analyzing tag.\n");
    analyze_tag(f);
    lint0--;
  }
}

void analyze_tiff(FILE *f) {
  printf("Inside analyze_tiff\n");
  char randString[7] = {0x45, 0x78, 0x69, 0x66, 0x00, 0x00};
  char bigEndian[3] = {0x4d, 0x4d};
  char lilEndian[3] = {0x49, 0x49};
  char magicString[3] = {0x2a, 0x00};
  char offset[5] = {0x08, 0x00, 0x00, 0x00};

  unsigned char* tiffbuff;
  tiffbuff = calloc(14, sizeof(unsigned char));
  //this is endianness, 2 bytes
  fread(tiffbuff, 1, 14, f);
  //printf("Tiffbuff contents: %02X%02X\n", tiffbuff[0], tiffbuff[1]);
  if (strncmp((char *)tiffbuff,randString, 6)) {
    printf("Error on randString\n");
  }
  if (!strncmp((char *)tiffbuff+6,lilEndian, 2)) {
    printf("Little endian. All is well.\n");
  } else if (!strncmp((char *)tiffbuff+6,bigEndian, 2)) {
    printf("Big endian. This is an error.\n");
  }
  //printf("Tiffbuff contents: %02X%02X\n", tiffbuff[0], tiffbuff[1]);
  if (!strncmp((char *)tiffbuff+8,magicString, 2)) {
    printf("Magic string matches.\n");
  } else {
    printf("Magical error on magical string.\n");
  }
  if (!strncmp((char *)tiffbuff+10,offset, 4)) {
    analyze_IFD_0(f); 
  } else {
    printf("Offset is not 8\n");
  }
  free(tiffbuff);
  return;

}

void superChunk_walk(FILE *f) {
  char chunk_front[2] = {0xff};
  char empty_chunk[2] = {0x00};
  unsigned char* parser;
  parser = calloc(1, sizeof(unsigned char));
  while(fread(parser, 1, 1, f)) {
    if (!strcmp((char *)parser,chunk_front)) {      
      printf("Chunk front in super chunk: %02X\n", parser[0]);
      fread(parser, 1, 1, f);
      if (strcmp((char *)parser,empty_chunk)) {
        printf("Not an empty chunk: %02X\n", parser[0]);
        fseek(f, (-2), SEEK_CUR);
        free(parser);
        return; 
      }
    }
  } 
}

long int standardLength(unsigned char* buffer) {
  unsigned char* buffer2;
  buffer2 = malloc(2*sizeof(unsigned char));
  printf("Buffer2 is: %02X%02X\n", buffer[0], buffer[1]);
  sprintf((char *) buffer2, "%02x%02X", buffer[0], buffer[1]);
  char *end;
  long int lint0 = strtol((char *) buffer2, &end, 16);
  free(buffer2);
  printf("Length is: %li\n", lint0);
  return lint0;
} 

void analyze_jpgchunks(FILE *f) {
  //Marker is 2 bytes
  //length is 2 bytes
  //data is (length-2) bytes
  char chunk_front[2] = {0xff};
  
  char superCd0[2] = {0xd0};
  char superCd1[2] = {0xd1};
  char superCd2[2] = {0xd2};
  char superCd3[2] = {0xd3};
  char superCd4[2] = {0xd4};
  char superCd5[2] = {0xd5};
  char superCd6[2] = {0xd6};
  char superCd7[2] = {0xd7};
  char superCd8[2] = {0xd8};
  char superCda[2] = {0xda};

  char super_chunk_end[2] = {0xd9};
  char APP1[2] = {0xe1};

  unsigned char *buffer;
  buffer = malloc(1*sizeof(unsigned char));
  while(fread(buffer, 1, 1, f) == 1) {
    if (strcmp((char *)buffer, chunk_front) == 0) {
      printf("Matched front of chunk\n");
      fread(buffer, 1, 1, f);
      if (strcmp((char *)buffer, APP1) == 0) {
        printf("APP1 found\n");
        printf("Buffer contents: %02X\n", buffer[0]);
        unsigned char *lengthBuffer;
        lengthBuffer = malloc(2*sizeof(unsigned char));
        fread(lengthBuffer, 1, 2, f);
        printf("New lengthbuffer contents: %02X%02X\n", lengthBuffer[0], lengthBuffer[1]);
        int chunkLength;
        chunkLength = standardLength(lengthBuffer);
        printf("Returned chunklength is %i\n", chunkLength);
        free(lengthBuffer);
        analyze_tiff(f);
      } else if (!strcmp((char *)buffer,superCd0) || !strcmp((char *)buffer,superCd1) || !strcmp((char *)buffer,superCd2)
                  || !strcmp((char *)buffer,superCd3) || !strcmp((char *)buffer,superCd4) || !strcmp((char *)buffer,superCd5)
                  || !strcmp((char *)buffer,superCd6) || !strcmp((char *)buffer,superCd7) || !strcmp((char *)buffer,superCd8)
                  || !strcmp((char *)buffer,superCda)) {
        printf("Superchunk engaged.\n");
        superChunk_walk(f);
      } else if (strcmp((char *)buffer, super_chunk_end) == 0) {
        printf("End of file\n");
        return;
      } else {
        printf("Standard chunk. Finding length.\n");
        printf("Buffer contents: %02X\n", buffer[0]);
        unsigned char *lengthBuffer;
        lengthBuffer = malloc(2*sizeof(unsigned char));
        fread(lengthBuffer, 1, 2, f);
        printf("New lengthbuffer contents: %02X%02X\n", lengthBuffer[0], lengthBuffer[1]);
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
  if (!strcmp((char *)buffer,(char *)jpg_SOI)) {
    fseek(f, -2, SEEK_CUR);
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
