/**
 * @file hash.c
 * @author Eric Smith ejsmit22
 * Contains main function for sha256 hash program, calls all other functions
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sha256.h"

// max arguments accepted
#define MAXARGS 2
// min arguments accepted
#define MINARGS 1

/**
 * main function, calls all other functions
 * @return exit code
*/
int main(int argc, char *argv[])
{
  if(argc > MAXARGS){
    fprintf( stderr, "usage: hash [input_file]\n" );
    exit( EXIT_FAILURE );
  }

  FILE *input;
  if(argc == MINARGS){
    input = freopen( NULL, "rb", stdin );
  } else {
    input = fopen(argv[MINARGS], "rb");
    
  }
  if(!input){
    fprintf( stderr, "%s: No such file or directory\n", argv[MINARGS] );
    exit( EXIT_FAILURE );
  }

  SHAState *state = makeState();
  byte thing[1024];
  int len;

  while((len = fread(thing, 1, sizeof(thing), input)) != 0){
    update(state, thing, len);
    
  }
  word hash[HASH_WORDS];

  digest(state, hash);
  for(int i = 0; i < HASH_WORDS; i++){
    printf("%08x", hash[i]);
  }
  printf("\n");

  fclose(input);




}