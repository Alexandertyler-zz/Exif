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

void print_tag(FILE *f, unsigned char * offsetbuff, int skip_offset, int length, int tiff) {
  unsigned char *buffer;
  unsigned char ASCII_string[8] = {0x41, 0x53, 0x43, 0x49, 0x49, 0x00, 0x00, 0x00};
  if(skip_offset) {
    length = 4;
    printf("%s\n", offsetbuff);
  } 
  else {  
    int offset = (int) tagLength(offsetbuff);
    int curr = ftell(f);
    //printf("current position: %d, offset from tiff: %d", curr, offset);
    curr -= tiff;
    fseek(f,(offset-curr),SEEK_CUR);    
    buffer = calloc(length+1, sizeof(char));
    if(fread(buffer,1,length,f) == length) {
      if(!strncmp((char *) buffer, (char *) ASCII_string, 8)) {
	printf("%s\n",buffer+8);
      }
      else {
      printf("%s\n", buffer);
      }
      fseek(f,-(offset-curr)-length,SEEK_CUR);
      free(buffer);
    } else {
      printf("ERROR.");
      free(buffer);
      //retval = -1;
    }  
  }
}
  
void handle_exif(FILE *f, unsigned char * offsetbuff, int tiff) {
  int curr_pos = ftell(f);
  int offset = (int) tagLength(offsetbuff);
  curr_pos -= tiff;
  int loc = ftell(f);
  fseek(f, (offset - curr_pos), SEEK_CUR);
  //printf("handleExif.\n");
  analyze_IFD(f, tiff);
  rewind(f);
  fseek(f, loc+12, SEEK_CUR);
}


void analyze_tag(FILE *f, int tiff) {
  //reverse these
  char DocName[3] = {0x0d, 0x01};
  char ImgDesc[3] = {0x0e, 0x01};
  char Make[3] = {0x0f, 0x01};
  char Model[3] = {0x10, 0x01};
  char Software[3] = {0x31, 0x01};
  char DateTime[3] = {0x32, 0x01};
  char Artist[3] = {0x3b, 0x01};
  char HostPC[3] = {0x3c, 0x01};
  char Copyright[3] = {0x98, 0x82};
  char RSF[3] = {0x04, 0xa0};
  char DTOriginal[3] = {0x03, 0x90};
  char DTDigitized[3] = {0x04, 0x90};
  char MakerNote[3] = {0x7c, 0x92};
  char UserComment[3] = {0x86, 0x92};
  char ImageUniqueID[3] = {0x20, 0xa4};
  char ExifIFD[3] = {0x69, 0x87};

  unsigned char* tagbuff;
  unsigned char* lenbuff;
  unsigned char* offsetbuff;
  //unsigned char* exifbuff;
  int length;
  // tag name and data type
  tagbuff = calloc(4, sizeof(unsigned char));
  // count
  lenbuff = calloc(4, sizeof(unsigned char));
  // offset
  offsetbuff = calloc(4, sizeof(char));
  
  fread(tagbuff, 1, 4, f);
  //printf("tagbuff: %02X%02X%02X%02X\n", tagbuff[0], tagbuff[1], tagbuff[2], tagbuff[3]);
  fread(lenbuff, 1, 4, f);
  //printf("lenbuff: %02X%02X%02X%02X\n", lenbuff[0], lenbuff[1], lenbuff[2], lenbuff[3]);
  fread(offsetbuff, 1, 4, f);
  //printf("offsetbuff: %02X%02X%02X%02X\n", offsetbuff[0], offsetbuff[1], offsetbuff[2], offsetbuff[3]);
  length = (int) tagLength(lenbuff);
  int skip_offset = 0;
  if (length <= 4) {
    skip_offset = 1;
  }
  //retval = 1;
  if (!strncmp((char *)tagbuff,ExifIFD,2)) {
    //printf("Exif found.\n");
    handle_exif(f, offsetbuff, tiff);
    //strncpy(offsetbuff, exifbuff, 4);
  } else if (!strncmp((char *)tagbuff,DocName,2)) {
    printf("DocumentName: ");
    print_tag(f, offsetbuff, skip_offset, length, tiff);
  } else if (!strncmp((char *)tagbuff,ImgDesc,2)) {
    printf("ImageDescription: ");
    print_tag(f, offsetbuff, skip_offset, length, tiff);
  } else if (!strncmp((char *)tagbuff,Make,2)) {
    printf("Make: ");
    print_tag(f, offsetbuff, skip_offset, length, tiff);
  } else if (!strncmp((char *)tagbuff,Model,2)) {
    printf("Model: ");
    print_tag(f, offsetbuff, skip_offset, length, tiff);
  } else if (!strncmp((char *)tagbuff,Software,2)) {
    printf("Software: ");
    print_tag(f, offsetbuff, skip_offset, length, tiff);
  } else if (!strncmp((char *)tagbuff,DateTime,2)) {
    printf("DateTime: ");
    print_tag(f, offsetbuff, skip_offset, length, tiff);
  } else if (!strncmp((char *)tagbuff,Artist,2)) {
    printf("Artist: ");
    print_tag(f, offsetbuff, skip_offset, length, tiff);
  } else if (!strncmp((char *)tagbuff,HostPC,2)) {
    printf("HostComputer: ");
    print_tag(f, offsetbuff, skip_offset, length, tiff);
  } else if (!strncmp((char *)tagbuff,Copyright,2)) {
    printf("Copyright: ");
    print_tag(f, offsetbuff, skip_offset, length, tiff);
  } else if (!strncmp((char *)tagbuff,RSF,2)) {
    printf("RelatedSoundFile: ");
    print_tag(f, offsetbuff, skip_offset, length, tiff);
  } else if (!strncmp((char *)tagbuff,DTOriginal,2)) {
    printf("DateTimeOriginal: ");
    print_tag(f, offsetbuff, skip_offset, length, tiff);
  } else if (!strncmp((char *)tagbuff,DTDigitized,2)) {
    printf("DateTimeDigitized: ");
    print_tag(f, offsetbuff, skip_offset, length, tiff);
  } else if (!strncmp((char *)tagbuff,MakerNote,2)) {
    printf("MakerNote: ");
    print_tag(f, offsetbuff, skip_offset, length, tiff);
  } else if (!strncmp((char *)tagbuff,UserComment,2)) {
    printf("UserComment: ");
    //fseek(f, 8, SEEK_CUR);
    print_tag(f, offsetbuff, skip_offset, length, tiff);
  } else if (!strncmp((char *)tagbuff,ImageUniqueID,2)) {
    printf("ImageUniqueID: ");
    print_tag(f, offsetbuff, skip_offset, length, tiff);
  } else {
    //printf("NOT MATCHING SHIT: %02X:%02X\n",tagbuff[0],tagbuff[1]);
  }
}


void analyze_IFD(FILE *f, int tiff) {
  unsigned char* ifdbuff;
  unsigned char* lenbuff;
  ifdbuff = calloc(2, sizeof(unsigned char));
  fread(ifdbuff, 1, 2, f);
  lenbuff = calloc(2, sizeof(unsigned char));
  //printf("ifdbuff is: %02X%02X\n", ifdbuff[0], ifdbuff[1]);
  sprintf((char *) lenbuff, "%02x%02X", ifdbuff[1], ifdbuff[0]);
  char *end;
  long int lint0 = strtol((char *) lenbuff, &end, 16);
  free(lenbuff);
  free(ifdbuff);
  //printf("Number of tags is: %li\n", lint0);
  
  while (lint0 > 0) {
    //printf("Analyzing tag.\n");
    analyze_tag(f, tiff);
    lint0--;
  }
}

void analyze_tiff(FILE *f) {
  //printf("Inside analyze_tiff\n");
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
    //printf("Little endian. All is well.\n");
  } else if (!strncmp((char *)tiffbuff+6,bigEndian, 2)) {
    printf("Big endian. This is an error.\n");
  }
  //printf("Tiffbuff contents: %02X%02X\n", tiffbuff[0], tiffbuff[1]);
  if (!strncmp((char *)tiffbuff+8,magicString, 2)) {
    //printf("Magic string matches.\n");
  } else {
    printf("Magical error on magical string.\n");
  }
  int beginning_of_TIFF = ftell(f)-8;
  if (!strncmp((char *)tiffbuff+10,offset, 4)) {
    
    analyze_IFD(f, beginning_of_TIFF); 
  } else {
    //printf("Offset is not 8\n");
    analyze_IFD(f, beginning_of_TIFF);
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
      //printf("Chunk front in super chunk: %02X\n", parser[0]);
      fread(parser, 1, 1, f);
      if (strcmp((char *)parser,empty_chunk)) {
        //printf("Not an empty chunk: %02X\n", parser[0]);
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
  //printf("Buffer2 is: %02X%02X\n", buffer[0], buffer[1]);
  sprintf((char *) buffer2, "%02x%02X", buffer[0], buffer[1]);
  char *end;
  long int lint0 = strtol((char *) buffer2, &end, 16);
  free(buffer2);
  //printf("Length is: %li\n", lint0);
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
      //printf("Matched front of chunk\n");
      fread(buffer, 1, 1, f);
      if (strcmp((char *)buffer, APP1) == 0) {
        //printf("APP1 found\n");
        //printf("Buffer contents: %02X\n", buffer[0]);
        unsigned char *lengthBuffer;
        lengthBuffer = malloc(2*sizeof(unsigned char));
        fread(lengthBuffer, 1, 2, f);
        //printf("New lengthbuffer contents: %02X%02X\n", lengthBuffer[0], lengthBuffer[1]);
        int chunkLength;
        chunkLength = standardLength(lengthBuffer);
        //printf("Returned chunklength is %i\n", chunkLength);
	chunkLength--;
        free(lengthBuffer);
        analyze_tiff(f);
      } else if (!strcmp((char *)buffer,superCd0) || !strcmp((char *)buffer,superCd1) || !strcmp((char *)buffer,superCd2)
                  || !strcmp((char *)buffer,superCd3) || !strcmp((char *)buffer,superCd4) || !strcmp((char *)buffer,superCd5)
                  || !strcmp((char *)buffer,superCd6) || !strcmp((char *)buffer,superCd7) || !strcmp((char *)buffer,superCd8)
                  || !strcmp((char *)buffer,superCda)) {
        //printf("Superchunk engaged.\n");
        superChunk_walk(f);
      } else if (strcmp((char *)buffer, super_chunk_end) == 0) {
        //printf("End of file\n");
        return;
      } else {
        //printf("Standard chunk. Finding length.\n");
        //printf("Buffer contents: %02X\n", buffer[0]);
        unsigned char *lengthBuffer;
        lengthBuffer = malloc(2*sizeof(unsigned char));
        fread(lengthBuffer, 1, 2, f);
        //printf("New lengthbuffer contents: %02X%02X\n", lengthBuffer[0], lengthBuffer[1]);
        int chunkLength;
        chunkLength = standardLength(lengthBuffer);
        fseek(f, chunkLength-2, SEEK_CUR);
        free(lengthBuffer);
      }
      //getchar();
    } else {
      //printf("No chunk front match.\n");
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
    //printf("buffer is: %x\n", buffer[i]);
    //printf("jpg_SOI is: %x\n", jpg_SOI[i]);
    i++;
  }
  if (!strcmp((char *)buffer,(char *)jpg_SOI)) {
    fseek(f, -2, SEEK_CUR);
    analyze_jpgchunks(f);
    //printf("JPG Confirmed.\n");
    free(buffer);
    return 1;
  } else {
    //printf("We've lost contact with the jpg sir.\n");
    free(buffer);
    return -1;
  }
}
