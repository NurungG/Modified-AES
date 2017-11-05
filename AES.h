#ifndef _AES_H_
#define _AES_H_

#define KEY_SIZE 16
#define BLOCK_SIZE 16
#define ROUNDS 10
#define SBOX_SIZE 256
#define FINAL_VECTOR 0x15

#include <stdio.h>
#include "GF.h"

// Variable Declaration
uint8_t Plain[BLOCK_SIZE];
uint8_t Key[KEY_SIZE];
uint8_t Cipher[BLOCK_SIZE];
uint8_t Decrypt[BLOCK_SIZE];

// Function Declaration
void AES_SetRoundConstant();
void AES_SetSbox();
void AES_SetInverseSbox();

void AES_KeyExpansion();
void AES_Encryption();
void AES_Decryption();


#endif