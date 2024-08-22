/**
 * @file sha256.c
 * @author Eric Smith ejsmit22
 * Contains functions that help convert files to hashed lines
*/

#include "sha256.h"
#include <stdlib.h>

/** max value of pending in digest */
#define MAX_DIGEST 56


/**
 * This function rotates the given val right by the given number of bits and returns the result.
 * @param val value
 * @param bits num bits to rotate
 * @return rotated words
*/
word rotate(word val, int bits)
{
    word rotate = (val >> bits) | (val << (32-bits));
    return rotate;
}

/**
 * This function computes the Σ0() function described in the sha256 algorithm.
 * @param a word a
 * @return word
*/
word Sigma0(word a)
{
    return rotate(a, 2) ^ rotate(a, 13) ^ rotate(a, 22);
}

/**
 * This function computes the Σ1() function described in the sha256 algorithm.
 * @param e word e
 * @return word
*/
word Sigma1( word e )
{
    return rotate(e, 6) ^ rotate(e, 11) ^ rotate(e, 25);
}

/**
 * This function computes the Ch() function described in the sha256 algorithm.
 * @param e word e
 * @param f word f
 * @param g word g
 * @return word
*/
word ChFunction( word e, word f, word g )
{
    return (e & f) ^ (~e & g);
}

/**
 * This function computes the Ma() function described in the sha256 algorithm.
 * @param a word a
 * @param b word b
 * @param c word c
 * @return word
*/
word MaFunction( word a, word b, word c )
{
    return (a & b) ^ (a & c) ^ (b & c);
}

/**
 * This function extends the 64-byte block given in the pending array and stores the 64-word result in the w[] array.
 * @param pending pending array from state
 * @param w word array
*/
void extendMessage( byte const pending[ BLOCK_SIZE ], word w[ BLOCK_SIZE ] )
{
    int penI = 0;
    for(int i = 0; i < 16; i++){
        w[i] = pending[penI++] << 24 | pending[penI++] << 16 | pending[penI++] << 8 | pending[penI++];
    }
    for(int i = 16; i < BLOCK_SIZE; i++){
        int first = i - 16;
        int second = i - 15;
        int third = i - 7;
        int fourth = i - 2;

        word tempsec = (rotate(w[second], 7)) ^ (rotate(w[second], 18)) ^ ((w[second] >> 3));
        word tempfourth = (rotate(w[fourth], 17)) ^ (rotate(w[fourth], 19)) ^ ((w[fourth] >> 10));
        w[i] = w[first] + tempsec + w[third] + tempfourth;
    }
    
}

/**
 * This function performs the compression() operation for the SHA256 hash. 
 * It processes a 64 byte blocks in the pending array of state, updating the h[] values in state. 
 * This function is for internal use by the sha256 component. 
 * It is called by update() as it completes 64-byte blocks for processing. At the end of the input, 
 * it is called once or twice by the digest() function as it adds padding to the end of the input.
 * @param state state pointer
*/
void compression( SHAState *state )
{
    word w[BLOCK_SIZE];
    extendMessage(state->pending, w);
    word a = state->h[0];
    word b = state->h[1];
    word c = state->h[2];
    word d = state->h[3];
    word e = state->h[4];
    word f = state->h[5];
    word g = state->h[6];
    word h = state->h[7];
    for(int i = 0; i < BLOCK_SIZE; i++){
        word ch = ChFunction(e, f, g);
        word sigma1 = Sigma1(e);
        word ma = MaFunction(a, b, c);
        word sigma0 = Sigma0(a);

        word thing1 = h + ch + sigma1 + w[i] + constant_k[i];
        word thing2 = sigma0 + ma;

        h = g;
        g = f;
        f = e;
        e = d + thing1;
        d = c;
        c = b;
        b = a;
        a = thing1 + thing2;
        
        
        
        
        
        
        
    }

    state->h[0] += a;
    state->h[1] += b;
    state->h[2] += c;
    state->h[3] += d;
    state->h[4] += e;
    state->h[5] += f;
    state->h[6] += g;
    state->h[7] += h;
}

/**
 * This function allocates an instance of SHAState on the heap and initializes 
 * its fields in preparation for processing an input file. 
 * In particular, the values of the h[] array should be initialized 
 * to match the initial_h[] array from sha256constants and the pending
 * array and length fields should show that no bytes are waiting to be processed.
 * @return SHAState sha
*/
SHAState *makeState()
{
    SHAState *sha = (SHAState *)malloc(sizeof(SHAState));
    for(int i = 0; i < BBITS; i++){
        sha->h[i] = initial_h[i];
    }
    sha->pcount = 0;
    sha->totalcount = 0;
    return sha;
}

/**
 * This function is called when a SHAState struct is no longer needed. It frees all the resources used by the state.
 * @param state pointer to SHAState struct
*/
void freeState( SHAState *state )
{
    free(state);
}

/**
 * This function is called when new input data is available to be processed. 
 * The input data is given as an array of bytes in the data array, 
 * with the len parameter indicating how many bytes are in the array. 
 * This function can be called with any amount of data from the input,
 * possibly more than 64 bytes or less. This function collects input data 
 * into 64-byte blocks and processes each block via the compress function. 
 * There may be leftover bytes after the last 64-byte block is processed. 
 * These are left in the pending array to be processed in a subsequent call to update() or digest().
 * @param state pointer to SHAState struct
 * @param data data array
 * @param len len of data
*/
void update( SHAState *state, const byte data[], int len )
{
    for(int i = 0; i < len; i++){
        state->pending[state->pcount++] = data[i];
        if(state->pcount >= BLOCK_SIZE){
            compression(state);
            state->pcount = 0;
            state->totalcount += BLOCK_SIZE;
        }
    }
}

/**
 * This is called once, after all data has been read 
 * from the input and processed by the update() function. 
 * This adds padding to the input and processes the last block 
 * or two of the input and copies the final hash value to the given hash[] array.
 * @param state pointer to SHAState struct
 * @param hash hash array for final hash
*/
void digest( SHAState *state, word hash[ HASH_WORDS ] )
{
    state->totalcount += state->pcount;
    state->pending[state->pcount++] = 0x80;

    if(state->pcount > MAX_DIGEST){
        for(int i = state->pcount; i < BLOCK_SIZE; i++){
            state->pending[i] = 0;
        }
        compression(state);
        state->pcount = 0;
        for(int i = 0; i < MAX_DIGEST; i++){
            state->pending[i] = 0;
        }

        word64 size = state->totalcount * BBITS;
        for(int i = 63; i >= MAX_DIGEST; i--){
            state->pending[i] = size & 0xFF;
            size = size >> BBITS;
        }
        compression(state);
    } else {
        for(int i = state->pcount; i < MAX_DIGEST; i++){
            state->pending[i] = 0;
        }

        word64 size = state->totalcount * BBITS;
        for(int i = BLOCK_SIZE - 1; i > MAX_DIGEST - 1; i--){
            state->pending[i] = size & 0xFF;
            size = size >> BBITS;
        }


        compression(state);
    }

    for(int j = 0; j < HASH_WORDS; j++){
        hash[j] = state->h[j];
    }
}