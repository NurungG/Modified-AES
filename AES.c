#include "AES.h"


/* Private Variables */
static uint8_t RoundKey[ROUNDS + 1][KEY_SIZE];
static uint8_t RoundConstant[ROUNDS + 1];

static uint8_t Sbox[SBOX_SIZE];
static uint8_t Inverse_Sbox[SBOX_SIZE];



/* Private Functions */
static void PrintText(uint8_t *text) {
	// Use for printing 16bytes text(block) as hex form
	int i;
	for (i = 0; i < BLOCK_SIZE; i++) {
		printf("%02X ", text[i]);
	}
	puts("");
}

static void CopyText(uint8_t *src, uint8_t *dst) {
	// Use for Copying 16bytes text(block) 
	int i;
	for (i = 0; i < BLOCK_SIZE; i++) {
		dst[i] = src[i];
	}
}

static void PrintKey(uint8_t *key) {
	// Use for keys (the struint8_tture is totally same with PrintText function)
	int i;
	for (i = 0; i < KEY_SIZE; i++) {
		printf("%02X ", key[i]);
	}
	puts("");
}

static void CopyKey(uint8_t *src, uint8_t *dst) {
	// Use for keys (the struint8_tture is totally same with CopyText function)
	int i;
	for (i = 0; i < KEY_SIZE; i++) {
		dst[i] = src[i];
	}
}

static int Rfunction(uint8_t *input, int r) {
	uint8_t output[4];

	// Shifted bytes are substituted by the Sbox
	// Especially, first byte is weighted by XORing with RC
	output[0] = Sbox[input[1]] ^ RoundConstant[r];
	output[1] = Sbox[input[2]];
	output[2] = Sbox[input[3]];
	output[3] = Sbox[input[0]];

	// Return as int form	
	return *(int *)&output;
}

static void SubstituteBytes() {
	int i;

	// Substitute bytes using Sbox
	for (i = 0; i < BLOCK_SIZE; i++) {
		Cipher[i] = Sbox[Cipher[i]];
	}

	// Print result
	printf("SB: ");
	PrintText(Cipher);
}

static void ShiftRows() {
	uint8_t temp;

	// Shift left 1
	temp = Cipher[1];
	Cipher[1] = Cipher[5];
	Cipher[5] = Cipher[9];
	Cipher[9] = Cipher[13];
	Cipher[13] = temp;

	// Shift left 2
	temp = Cipher[2];
	Cipher[2] = Cipher[10];
	Cipher[10] = temp;
	temp = Cipher[6];
	Cipher[6] = Cipher[14];
	Cipher[14] = temp;

	// Shift left 3 (equals to Shift right 1)
	temp = Cipher[3];
	Cipher[3] = Cipher[15];
	Cipher[15] = Cipher[11];
	Cipher[11] = Cipher[7];
	Cipher[7] = temp;

	// Print result
	printf("SR: ");
	PrintText(Cipher);
}

static void MixColumns() {
	int i, j;
	uint8_t ret[BLOCK_SIZE];		// temporary field to store operation result


	// Multiplication of 4x4 matrix based on GF(2^8)
	/* Weighted matrix is
		[0x02, 0x03, 0x01, 0x01]
		[0x01, 0x02, 0x03, 0x01]
		[0x01, 0x01, 0x02, 0x03]
		[0x03, 0x01, 0x01, 0x02]  */
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			ret[i * 4 + j] = GF_addition(
				GF_multiplication(Cipher[i * 4 + j], 0x02), GF_addition(
					GF_multiplication(Cipher[i * 4 + ((j + 1) % 4)], 0x03), GF_addition(
						Cipher[i * 4 + ((j + 2) % 4)], Cipher[i * 4 + ((j + 3) % 4)]
					)
				)
			);
		}
	}

	// Push to real field
	CopyText(ret, Cipher);

	// Print result
	printf("MC: ");
	PrintText(Cipher);
}

static void AddRoundKey(uint8_t *text, int r) {
	int i;

	// XORing with Round Key
	for (i = 0; i < BLOCK_SIZE; i++) {
		text[i] = text[i] ^ RoundKey[r][i];
	}

	// Print result
	printf("AR: ");
	PrintText(text);
}

static void Inverse_SubBytes() {
	int i;

	// Substitute bytes using Inverse_Sbox
	for (i = 0; i < BLOCK_SIZE; i++) {
		Decrypt[i] = Inverse_Sbox[Decrypt[i]];
	}

	// Print result
	printf("SB: ");
	PrintText(Decrypt);
}

static void Inverse_ShiftRows() {
	uint8_t temp;

	// Shift right 1
	temp = Decrypt[1];
	Decrypt[1] = Decrypt[13];
	Decrypt[13] = Decrypt[9];
	Decrypt[9] = Decrypt[5];
	Decrypt[5] = temp;

	// Shift right 2
	temp = Decrypt[2];
	Decrypt[2] = Decrypt[10];
	Decrypt[10] = temp;
	temp = Decrypt[6];
	Decrypt[6] = Decrypt[14];
	Decrypt[14] = temp;

	// Shift right 3 (equals to Shift left 1)
	temp = Decrypt[3];
	Decrypt[3] = Decrypt[7];
	Decrypt[7] = Decrypt[11];
	Decrypt[11] = Decrypt[15];
	Decrypt[15] = temp;

	// Print result
	printf("SR: ");
	PrintText(Decrypt);
}

static void Inverse_MixCols() {
	int i, j;
	uint8_t ret[BLOCK_SIZE];		// temporary field to store operation result


	// Multiplication of 4x4 matrix based on GF(2^8)
	/* Weighted inverse matrix is
		[0x0E, 0x0B, 0x0D, 0x09]
		[0x09, 0x0E, 0x0B, 0x0D]
		[0x0D, 0x09, 0x0E, 0x0B]
		[0x0B, 0x0D, 0x09, 0x0E]  */
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			ret[i * 4 + j] = GF_addition(
				GF_multiplication(0x0E, Decrypt[i * 4 + j]), GF_addition(
					GF_multiplication(0x0B, Decrypt[i * 4 + ((j + 1) % 4)]), GF_addition(
						GF_multiplication(0x0D, Decrypt[i * 4 + ((j + 2) % 4)]),
						GF_multiplication(0x09, Decrypt[i * 4 + ((j + 3) % 4)])
					)
				)
			);
		}
	}	// 가독성이 오히려 더 떨어지는 것 같습니다 조교님...ㅠ

	// Push to real field
	CopyText(ret, Decrypt);

	// Print result
	printf("MC: ");
	PrintText(Decrypt);
}



/* Public Fuctions */
void AES_SetRoundConstant() {
	// Remember that RC is One-based array (To sync with rounds)
	int i;

	// Set Round Constants
	RoundConstant[0] = 0x00;
	RoundConstant[1] = 0x01;
	for (i = 2; i <= ROUNDS; i++) {
		// RC of current round is double of previous round
		RoundConstant[i] = GF_multiplication(RoundConstant[i - 1], 0x02);
	}

	// Print Round Constants
	printf("RC: ");
	for (i = 1; i <= ROUNDS; i++) {
		printf("%02X ", RoundConstant[i]);
	}
	puts("");
}

void AES_SetSbox() {
	int i, j, k;
	
	// Affine transformation matrix
	uint8_t Rows[8] = {
		0xF1,		// [11110001]
		0xE3,		// [11100011]
		0xC7,		// [11000111]
		0x8F,		// [10001111]
		0x1F,		// [00011111]
		0x3E,		// [00111110]
		0x7C,		// [01111100]
		0xF8		// [11111000]
	};

	// Set Sbox using Affine transformation calculating on GF(2^8)
	for (i = 0; i < SBOX_SIZE; i++) {
		uint8_t ret = 0x00;		// to be stored to Sbox
		for (j = 0; j < 8; j++) {
			// Calculate each row with multiplicative inverse of each ordered number on GF(2^8)
			uint8_t temp = Rows[j] & GF_getMulInverse(i);
			
			for (k = 0; k < 8; k++) {
				if (temp & (0x01 << k)) {
					ret = GF_addition(ret, 0x01 << j);
				}
			}
		}
		// Store the added result with Final vector
		Sbox[i] = GF_addition(ret, FINAL_VECTOR);
	}
}

void AES_SetInverseSbox() {
	int i, j, k;
	uint8_t inv_vec = 0x00;

	// Inverse Affine transformation matrix
	uint8_t Inverse_Rows[8] = {
		0xA4,		// [10100100]
		0x49,		// [01001001]
		0x92,		// [10010010]
		0x25,		// [00100101]
		0x4A,		// [01001010]
		0x94,		// [10010100]
		0x29,		// [00101001]
		0x52		// [01010010]
	};
	
	// Get inverse vector to be added on last step
	for (i = 0; i < 8; i++) {
		uint8_t temp = Inverse_Rows[i] & FINAL_VECTOR;
		for (j = 0; j < 8; j++) {
			if (temp & (0x01 << j)) {
				inv_vec = GF_addition(inv_vec, 0x01 << i);
			}
		}
	}

	// Set Inverse Sbox using inverse Affine transformation
	for (i = 0; i < SBOX_SIZE; i++) {
		uint8_t ret = 0x00;
		for (j = 0; j < 8; j++) {
			// Use raw element on inverse affine transformation
			uint8_t temp = Inverse_Rows[j] & i;
			for (k = 0; k < 8; k++) {
				if (temp & (0x01 << k)) {
					ret = GF_addition(ret, 0x01 << j);
				}
			}
		}
		// Store the multiplicative inverse (on GF(2^8)) of added result with inverse vector
		Inverse_Sbox[i] = GF_getMulInverse(GF_addition(ret, inv_vec));
	}
}

void AES_KeyExpansion() {
	int i;

	// Copy original Key to Round Key of round 0
	CopyKey(Key, RoundKey[0]);

	// Key Expansion
	for (i = 1; i <= ROUNDS; i++) {
		// Round key is seperated 4 columns
		// Each column is expanded by XORing the column adjacent to the left with the column on same position of previous round
		// Especially, the first column of each round is XORed with the previous round's last column that passed R-function
		*(int *)&RoundKey[i][0] = *(int *)&RoundKey[i - 1][0] ^ Rfunction(&RoundKey[i - 1][12], i);
		*(int *)&RoundKey[i][4] = *(int *)&RoundKey[i - 1][4] ^ *(int *)&RoundKey[i][0];
		*(int *)&RoundKey[i][8] = *(int *)&RoundKey[i - 1][8] ^ *(int *)&RoundKey[i][4];
		*(int *)&RoundKey[i][12] = *(int *)&RoundKey[i - 1][12] ^ *(int *)&RoundKey[i][8];
	}

	// Print expanded Round Keys
	puts("KEY EXPANSION");
	for (i = 0; i <= ROUNDS; i++) {
		printf("ROUND %d: ", i);
		PrintText(RoundKey[i]);
	}
	puts("");
}

void AES_Encryption() {
	int i;

	// Get the plain text to be encrypted
	CopyText(Plain, Cipher);

	// On Round 0, Add Round Key before processing main rounds
	puts("Round 0");
	AddRoundKey(Cipher, 0);
	puts("");

	for (i = 1; i < ROUNDS; i++) {
		// Repeat 4 steps (R1 ~ R9) -> 1.SB 2.SR 3.MC 4.AR
		printf("Round %d\n", i);
		SubstituteBytes();
		ShiftRows();
		MixColumns();
		AddRoundKey(Cipher, i);
		puts("");
	}

	// On last round, take 3 steps except MC
	puts("Round 10");
	SubstituteBytes();
	ShiftRows();
	AddRoundKey(Cipher, ROUNDS);
	puts("");
}

void AES_Decryption() {
	// Remember that a decryption has similar way to process with encryption
	// Change operation order, and use inverse functions
	int i;

	// Get the cipher text to be decrypted
	CopyText(Cipher, Decrypt);

	// On Round 0, Add Round Key before processing main rounds
	puts("Round 0");
	AddRoundKey(Decrypt, ROUNDS);
	puts("");

	for (i = 1; i < ROUNDS; i++) {
		// Repeat 4 steps (R1 ~ R9) -> 1.SR 2.SB 3.AR 4.MC
		// That's all inverse function, except AR
		printf("Round %d\n", i);
		Inverse_ShiftRows();
		Inverse_SubBytes();
		AddRoundKey(Decrypt, ROUNDS - i);
		Inverse_MixCols();
		puts("");
	}

	// On last round, take 3 steps except MC
	puts("Round 10");
	Inverse_ShiftRows();
	Inverse_SubBytes();
	AddRoundKey(Decrypt, 0);
	puts("");
}
