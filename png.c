#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <zlib.h>
#include "png.h"

int printzTXt(FILE *f, unsigned char *chunktype, int chunklength) {
  //Declarations
  int retval;

  //fread buffer
  unsigned char *buffer;
  buffer = (unsigned char *) malloc((chunklength+4)*sizeof(char));

  unsigned char *check_sum;
  check_sum = (unsigned char*) malloc(4*sizeof(char));

  //Plaintext value/key parts of text chunk, maximum length is chunklength
  unsigned char *value_segment;
  value_segment = (unsigned char *) malloc(chunklength*sizeof(char));
  unsigned char *key_segment;
  key_segment = (unsigned char *) malloc(chunklength*sizeof(char));

  //Start with a 1000 byte keylen, increase if needed later in function
  uLongf keylen = 1000;

  //uncompress() strange error on some lines, decided to calloc to be sure %s knows when to end string
  char *uncompressed_key;
  uncompressed_key = calloc(keylen, sizeof(unsigned char));



  if (fread(buffer,1,chunklength+4,f) == (chunklength+4)) {
    retval = 1;
    int counter = 0;

    //Process value and look for null
    while (buffer[counter] != '\0' && counter < chunklength) {
      sprintf((char *) key_segment+counter, "%c", (unsigned char) buffer[counter]);
      counter++;
    }

    //Copy over null
    sprintf((char *) key_segment+counter, "%c", (unsigned char) buffer[counter]);

    //Increment past null and compression type
    counter++;
    counter++;

    printf("%s: ", key_segment);

    //Make sure that there was actually a null character
    if (counter == chunklength) {
      printf("Error, improperly formatted text chunk");
      retval = -1;
    }

    //Process key
    uLongf tracker = 0;
    while (counter < chunklength) {
      sprintf((char *) value_segment+tracker, "%c", buffer[counter]);
      counter++;
      tracker++;
    }

    //Attempt to decompress, if fail, reallocated uncompressed key
    while(uncompress((unsigned char*) uncompressed_key, &keylen, (unsigned char*) value_segment, tracker) == -5) {
      free(uncompressed_key);
      keylen = keylen * 2;
      uncompressed_key = calloc(keylen,sizeof(unsigned char));
    }
    printf("%s\n",uncompressed_key);

    //Check checksum
    sprintf((char *) check_sum,"%02X%02X%02X%02X", (unsigned char) buffer[chunklength], (unsigned char) buffer[chunklength+1], (unsigned char) buffer[chunklength+2], (unsigned char) buffer[chunklength+3]);
    int checksum = (int) strtol((char *) check_sum, NULL, 16);
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
  // Free everything
  free(uncompressed_key);
  free(value_segment);
  free(key_segment);
  free(buffer);
  free(check_sum);
  return retval;
}
  
int printtEXt(FILE *f, unsigned char *chunktype, int chunklength) {
  //Declarations
  int retval;
  unsigned char *buffer;
  buffer = (unsigned char *) malloc((chunklength+4)*sizeof(char));
  unsigned char *check_sum;
  check_sum = (unsigned char *) malloc(4*sizeof(char));
  unsigned char *value_segment;
  unsigned char *key_segment;
  key_segment = (unsigned char *) malloc(chunklength*sizeof(char));
  value_segment = (unsigned char *) malloc(chunklength*sizeof(char));

  if (fread(buffer,1,chunklength+4,f) == (chunklength+4)) {
    retval = 1;
    int counter = 0;
    //Process buffer and look for null
    while (buffer[counter] != '\0' && counter < chunklength) {
      sprintf((char *) key_segment+counter, "%c", (unsigned char) buffer[counter]);
      counter++;
    }
    //Copy over null
    sprintf((char *) key_segment+counter, "%c", (unsigned char) buffer[counter]);
    //Increment past null
    counter++;

    printf("%s: ", key_segment);
    //Make sure that there was actually a null character
    if (counter == chunklength) {
      printf("Error, improperly formatted text chunk");
      retval = -1;
    }

    int tracker = 0;
    while (counter < chunklength) {
      sprintf((char *) value_segment+tracker, "%c", (unsigned char) buffer[counter]);
      counter++;
      tracker++;
    }
    printf("%s\n",value_segment);

    //Check checksum
    sprintf((char *) check_sum,"%02X%02X%02X%02X", (unsigned char) buffer[chunklength], (unsigned char) buffer[chunklength+1], (unsigned char) buffer[chunklength+2], (unsigned char) buffer[chunklength+3]);
    int checksum = (int) strtol((char *) check_sum, NULL, 16);
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
    
int printtIMEstamp(FILE *f, unsigned char *chunktype, int chunklength) {
  unsigned char *buffer;
  unsigned char *year_array;
  unsigned char *check_sum;
  int year;
  int retval; 
  year_array = (unsigned char *) malloc(2*sizeof(char));
  buffer = (unsigned char *) malloc(11*sizeof(char));
  check_sum = (unsigned char *) malloc(4*sizeof(char));
  if (fread(buffer,1,11,f) == 11) { 
    retval = 1;

    // Formatting
    sprintf((char *) year_array,"%02X%02X", (unsigned char) buffer[0], (unsigned char) buffer[1]);
    year = (int) strtol((char *) year_array, NULL, 16);

    printf("Timestamp: %d/%d/%d %d:%d:%d\n", buffer[2], buffer[3], year, buffer[4], buffer[5], buffer[6]);

    // Check checksum  
    sprintf((char *) check_sum,"%02X%02X%02X%02X", (unsigned char) buffer[7], (unsigned char) buffer[8], (unsigned char) buffer[9], (unsigned char) buffer[10]);
    int checksum = (int) strtol((char *) check_sum, NULL, 16);
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

long int chunk_length(unsigned char* buffer) {
  /*HOW IT WORKS: Take our initial buffer, store the values into a new buffer
  with leading 0s using the command sprintf. Then use strtol to convert
  our new string "buffer2" into int value using base 16."*/

  unsigned char *buffer2;
  buffer2 = malloc(4*sizeof(char));

  sprintf((char *) buffer2, "%02X%02X%02X%02X",(unsigned char) buffer[0],(unsigned char) buffer[1],(unsigned char) buffer[2],(unsigned char) buffer[3]);

  long int lint0 = strtol((char *) buffer2, NULL, 16);  

  free(buffer2);
 
  return lint0;
}


int analyze_chunks(FILE *f) {
  unsigned char tEXt[4] = {0x74, 0x45, 0x58, 0x74};
  unsigned char zTXt[4] = {0x7a, 0x54, 0x58, 0x74};
  unsigned char tIME[4] = {0x74, 0x49, 0x4d, 0x45};

  int retval; 
  /* Idea for proceeding: we need to go through chunks of code. Chunks take the form:
     length of data portion of chunk (4 bytes) chunktype (4bytes) data (variable bytes) checksum (4 bytes) 
     So process first 8 bytes of next chunk. If the last 4 bytes match chunktypes above, then process
     and output data portion (after verifying checksum). Otherwise skip ahead the length of the data 
     + 4 bytes. */

  /* This function only analyzes 8 bytes at a time */
  unsigned char *buffer;
  buffer = (unsigned char *) malloc(8*sizeof(char));
  
  int chunklength;  
  while(fread(buffer, 1, 8, f) == 8 && retval != -1) {
  
    chunklength = chunk_length(buffer);

    //Opted for strncmp here because wouldn't match with strcmp
    if (strncmp((char *) buffer+4, (char *) tEXt,4) == 0) {
      retval = printtEXt(f, buffer+4, chunklength);
    }
    else if (strncmp((char *) buffer+4, (char * ) zTXt,4) == 0) {
      retval = printzTXt(f, buffer+4, chunklength);
    }
    else if (strncmp((char *) buffer+4, (char *) tIME,4) == 0) {
      retval = printtIMEstamp(f, buffer+4, chunklength);
    } 
    else {   
      fseek(f,chunklength+4, SEEK_CUR);
    }
  }
  if (feof(f)) {
    //We've hit the end of the file.
    free(buffer);
    return retval;
  }
  else {
    //Something bad happened
    printf("Error reading file.");
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
  unsigned char png_header[8] = {0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a};
  int retval;
  //Read in first 8 bytes and compare with standard PNG intro to determine if PNG file 
  unsigned char *buffer;
  buffer = (unsigned char *) malloc(8*sizeof(char));

  //Reads into buffer, each element 1 byte, read 8 bytes, read from input stream f
  //Makes sure the header matches PNG and fread was successful
  if ((fread(buffer, 1, 8, f) == 8) && (strcmp((char *) buffer, (char *) png_header) == 0)) {
    retval = analyze_chunks(f);
    free(buffer);
    if (retval != -1) {
      return 1;
    }
    else {
      return -1;
    }
  } else {
    printf("Not a valid PNG file or read error.\n");
    free(buffer);
    return -1;
  }
}

 
