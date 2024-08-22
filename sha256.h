/**
 */

#ifndef SHA256_H
#define SHA256_H

#include "sha256constants.h"

/** Type used to represent a byte. */
typedef unsigned char byte;

/** Type used to represent a 64-bit value. */
typedef unsigned long word64;

/** Number of bits in a byte. */
#define BBITS 8

/** Size of an input block in bytes. */
#define BLOCK_SIZE 64

/** Size of the hash, in words. */
#define HASH_WORDS 8

/** State of the SHA256 algorithm, including bytes of input data
    waiting to be hashed. */
typedef struct {
  /** Input data not yet hashed. */
  byte pending[ BLOCK_SIZE ];

  /** Number of byes currently in the pending array. */
  int pcount;

  // Add any fields you need.
  
  /** total count of bytes processed */
  int totalcount;

  /** Current hash value. */
  word h[ HASH_WORDS ];
} SHAState;

SHAState *makeState();

void freeState( SHAState *state );

void update( SHAState *state, const byte data[], int len );

void digest( SHAState *state, word hash[ HASH_WORDS ] );


#endif
