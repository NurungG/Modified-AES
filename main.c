#include "AES.h"

void ReadBinary(char *filename, uint8_t *buf, int size);
void WriteBinary(char *filename, uint8_t *buf, int size);

int main() {
	// Initiate RoundConstants, Sbox, and Inverse_Sbox
	AES_SetRoundConstant();
	AES_SetSbox();
	AES_SetInverseSbox();

	// Get a plaintext
	printf("PLAIN: ");
	ReadBinary("plain.bin", Plain, BLOCK_SIZE);

	// Get a key
	printf("KEY: ");
	ReadBinary("key.bin", Key, KEY_SIZE);


	// Encryption
	puts("\n<------ ENCRYPTION ------>\n");

	// At first, set round keys by key expansion
	AES_KeyExpansion();

	// Then encrypt & write ciphertext to file
	AES_Encryption();
	printf("CIPHER: "); 
	WriteBinary("cipher.bin", Cipher, BLOCK_SIZE);


	// Decryption
	puts("\n\n<------ DECRYPTION ------>\n");

	// Decrypt & write Decrypted text to file
	AES_Decryption();
	printf("DECRYPTED: ");
	WriteBinary("decrypt.bin", Decrypt, BLOCK_SIZE);

	return 0;
}

void ReadBinary(char *filename, uint8_t *buf, int size) {
	FILE *fp;
	int i;
	
	// File open & read as binary mode
	fp = fopen(filename, "rb");
	fread(buf, sizeof(uint8_t), size, fp);
	fclose(fp);

	// Print file contents
	for (i = 0; i < size; i++) {
		printf("%02X ", buf[i]);
	}
	puts("");
}

void WriteBinary(char *filename, uint8_t *buf, int size) {
	FILE *fp;
	int i;

	// File open & write as binary mode
	fp = fopen(filename, "wb");
	fwrite(buf, sizeof(uint8_t), size, fp);
	fclose(fp);

	// Print file contents
	for (i = 0; i < size; i++) {
		printf("%02X ", buf[i]);
	}
	puts("");
}