#include "AES.h"
#include <time.h>
#include <stdlib.h>

int init_flag = 0;
int pos = BLOCK_SIZE;

void init_AES() {
	AES_SetRoundConstant();
	AES_SetSbox();
	AES_KeyExpansion();

	srand((unsigned int)time(0));
}

void* AES(void *v, int size) {
	int i;
	uint8_t *value;

	if (!init_flag) {
		init_AES();
		init_flag = 1;
	}
	
	value = (uint8_t *)v;

	for (i = 0; i < size; i++) {
		if (pos >= BLOCK_SIZE) {
			for (int j; j < BLOCK_SIZE; j++) {
				Plain[j] = rand() % 0x100; // 0x00..0xFF
			}
			AES_Encryption();
			pos = 0;
		}
		value[i] ^= Cipher[pos++];
	}

	return (void *)value;
}


int main() {
	uint8_t value[4] = { 0x12, 0x34, 0x56, 0x78 };
	int size = 4;
	uint8_t *ret;

	int i;

	for(int t=0; t<100; t++){
		ret = (uint8_t *)AES((void *)value, size);
		for (i = 0; i < size; i++) {
			printf("%02x ", value[i]);
		}
		puts("\n");
	}

	return 0;
}
