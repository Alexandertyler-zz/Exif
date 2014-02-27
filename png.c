#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <zlib.h>
#include "png.h"
int printzTXt(FILE *f, char *chunktype, int chunklength) {
  //Declarations
  int retval;
  char *buffer;
  buffer = malloc((chunklength+4)*sizeof(char));
  char *check_sum;
  check_sum = malloc(4*sizeof(char));
  unsigned char *value_segment;
  unsigned char *key_segment;
  unsigned char *uncompressed_key;
  uLongf keylen = 1000;
  uncompressed_key = (unsigned char *) malloc(keylen*sizeof(unsigned char));
  value_segment = (unsigned char *) malloc(chunklength*sizeof(unsigned char));
  //Not sure how large to make this section so can't be greater than chunklength
  key_segment = malloc(chunklength*sizeof(char));
  if (fread(buffer,1,chunklength+4,f) == (chunklength+4)) {
    retval = 1;
    int counter = 0;
    //Process buffer and look for null
    while (buffer[counter] != '\0' && counter < chunklength) {
      sprintf(key_segment+counter, "%c", (unsigned char) buffer[counter]);
      counter++;
    }
    //Copy over null
    sprintf(key_segment+counter, "%c", (unsigned char) buffer[counter]);
    //Increment past null
    counter++;
    counter++;
    printf("%s: ", key_segment);
    //Make sure that there was actually a null character
    if (counter == chunklength) {
      printf("Error, improperly formatted text chunk");
      retval = -1;
    }
    //Initialize val segment
    
    uLongf tracker = 0;
    
    while (counter < chunklength) {
      sprintf(value_segment+tracker, "%c", (unsigned char) buffer[counter]);
      counter++;
      tracker++;
    }

    uncompress((unsigned char*) uncompressed_key, &keylen, (unsigned char*) value_segment, tracker);
    printf("%s\n",uncompressed_key);

    //Check checksum
    sprintf(check_sum,"%02X%02X%02X%02X", (unsigned char) buffer[chunklength], (unsigned char) buffer[chunklength+1], (unsigned char) buffer[chunklength+2], (unsigned char) buffer[chunklength+3]);
    int checksum = (int) strtol(check_sum, NULL, 16);
    unsigned int calculated_checksum = crc32(0, Z_NULL, 0);
    calculated_checksum = crc32(calculated_checksum, (unsigned char*) chunktype, 4);
    calculated_checksum = crc32(calculated_checksum, (unsigned char*) buffer, chunklength);
    if (calculated_checksum != checksum) {
      printf("Error, incorrect checksum.");
      retval = -1;
    }
  }
  //Couldnt read length of chunks
  else {
    printf("Error, reached EOF in text chunk.");
    retval = -1;
  }  
  // make sure these are all created outside 
  free(uncompressed_key);
  free(value_segment);
  free(key_segment);
  free(buffer);
  free(check_sum);
  return retval;
}
  


int printtEXt(FILE *f, char *chunktype, int chunklength) {
  //Declarations
  int retval;
  char *buffer;
  buffer = malloc((chunklength+4)*sizeof(char));
  char *check_sum;
  check_sum = malloc(4*sizeof(char));
  char *value_segment;
  char *key_segment;
  //Not sure how large to make this section so can't be greater than chunklength
  key_segment = malloc(chunklength*sizeof(char));
  if (fread(buffer,1,chunklength+4,f) == (chunklength+4)) {
    retval = 1;
    int counter = 0;
    //Process buffer and look for null
    while (buffer[counter] != '\0' && counter < chunklength) {
      sprintf(key_segment+counter, "%c", (unsigned char) buffer[counter]);
      counter++;
    }
    //Copy over null
    sprintf(key_segment+counter, "%c", (unsigned char) buffer[counter]);
    //Increment past null
    counter++;
    printf("%s: ", key_segment);
    //Make sure that there was actually a null character
    if (counter == chunklength) {
      printf("Error, improperly formatted text chunk");
      retval = -1;
    }
    //Initialize val segment
    value_segment = malloc((chunklength-counter)*sizeof(char));
    int tracker = 0;
    while (counter < chunklength) {
      sprintf(value_segment+tracker, "%c", (unsigned char) buffer[counter]);
      counter++;
      tracker++;
    }
    printf("%s\n",value_segment);

    //Check checksum
    sprintf(check_sum,"%02X%02X%02X%02X", (unsigned char) buffer[chunklength], (unsigned char) buffer[chunklength+1], (unsigned char) buffer[chunklength+2], (unsigned char) buffer[chunklength+3]);
    int checksum = (int) strtol(check_sum, NULL, 16);
    unsigned int calculated_checksum = crc32(0, Z_NULL, 0);
    calculated_checksum = crc32(calculated_checksum, (unsigned char*) chunktype, 4);
    calculated_checksum = crc32(calculated_checksum, (unsigned char*) buffer, chunklength);
    if (calculated_checksum != checksum) {
      printf("Error, incorrect checksum.");
      retval = -1;
    }
  }
  //Couldnt read length of chunks
  else {
    printf("Error, reached EOF in text chunk.");
    retval = -1;
  }  
  free(value_segment);
  free(key_segment);
  free(buffer);
  free(check_sum);
  return retval;
}
    
int printtIMEstamp(FILE *f, char *chunktype, int chunklength) {
  char *buffer;
  char *year_array;
  char *check_sum;
  int year;
  int retval; 
  year_array = malloc(2*sizeof(char));
  buffer = malloc(11*sizeof(char));
  check_sum = malloc(4*sizeof(char));
  if (fread(buffer,1,11,f) == 11) { 
    retval = 1;
    // Formatting
    sprintf(year_array,"%02X%02X", (unsigned char) buffer[0], (unsigned char) buffer[1]);
    year = (int) strtol(year_array, NULL, 16);
    printf("Timestamp: %d/%d/%d %d:%d:%d\n", buffer[2], buffer[3], year, buffer[4], buffer[5], buffer[6]);
    // Check checksum  
    sprintf(check_sum,"%02X%02X%02X%02X", (unsigned char) buffer[7], (unsigned char) buffer[8], (unsigned char) buffer[9], (unsigned char) buffer[10]);
    int checksum = (int) strtol(check_sum, NULL, 16);
    unsigned int calculated_checksum = crc32(0, Z_NULL, 0);
    calculated_checksum = crc32(calculated_checksum, (unsigned char*) chunktype, 4);
    calculated_checksum = crc32(calculated_checksum, (unsigned char*) buffer, chunklength);
    if (calculated_checksum != checksum) {
      printf("Error, incorrect checksum.");
      retval = -1;
    }
  }
  else {
    printf("Error, incorrectly formatted tIME chunk");
    retval = -1;
  }
  free(buffer);
  free(year_array);
  free(check_sum);
  return retval;
}

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
  int retval; 
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
  while(fread(buffer, 1, 8, f) == 8 && retval != -1) {
    //int i = 0;
    chunklength = chunk_length(buffer);
    //chunklength = 0;
    /*
    int i = 4;
    while ( i < 8) {
      printf("%X:",(unsigned char) buffer[i] );
      //printf("%x",tEXt[i-4]);
      //printf("%x",zTXt[i-4]);
      //printf("%x",tIME[i-4]);
      i++;
    } 
    */
   
    if (strncmp(buffer+4, tEXt,4) == 0) {
      //printf("ITS A TEXT!");
      retval = printtEXt(f, buffer+4, chunklength);
    }
    else if (strncmp(buffer+4, zTXt,4) == 0) {
      //printf("We gots a zTXt. Decompress that shit.\n");
      retval = printzTXt(f, buffer+4, chunklength);
    }
    else if (strncmp(buffer+4, tIME,4) == 0) {
      retval = printtIMEstamp(f, buffer+4, chunklength);
    } else {   
      fseek(f,chunklength+4, SEEK_CUR);
    }
    //getchar();
  }
  if (feof(f)) {
    //We've hit the end of the file.
    free(buffer);
    return retval;
  }
  else {
    //Something bad happened
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
    //printf("THIS SHITS A PNG!\n");
    return 1;
  } else {
    printf("Not even a little bit of a PNG.\n");
    return -1;
  }
}

 
