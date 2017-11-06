#include "GF.h"

/* Private Variables */
static uint8_t carry[7];		// 7 elements to be substituted (x^8 ~ x^14)
								// It has maximum degree on (x^7)*(x^7), because there are no addition carries


/* Private Functions */
static void SetCarry() {
	int i, j;
	for (i = 0; i < 7; i++) {
		// Shift from x^8 = 0x4D
		uint8_t temp = IRR_POLY << i;
		for (j = 1; j <= i; j++) {
			// Add carries by substitute operation
			temp = GF_addition(temp, ((IRR_POLY >> 8 - j) & 0x01) * carry[i - j]);
		}
		// Write memo
		carry[i] = temp;
	}
}

static int getMsb(uint8_t x) {
	int i = 0;

	// If there are no bit set, return -1
	if (x == 0x00) return -1;

	// Find MSB
	while (((0x80 >> i++) & x) == 0) {
		// empty
	}
	return (8 - i);
}

static void swap(uint8_t *a, uint8_t *b) {
	uint8_t tmp = *a;
	*a = *b;
	*b = tmp;
}


/* Public Functions */
uint8_t GF_addition(uint8_t a, uint8_t b) {
	// Addition in GF(2^8) completely equals to XOR operation
	return a ^ b;
}

uint8_t GF_multiplication(uint8_t a, uint8_t b) {
	// Let's see a as multiplicand, b as multiplier
	int i, j;
	uint8_t ret = 0x00;

	// If the carry is not defined, initiate carries
	if (carry[0] == 0) {
		SetCarry();
	}

	// Multiply
	for (i = 0; i < 8; i++) {
		if ((b >> i) & 0x01) {
			// If multiplier's bit is set, add shifted result
			ret = GF_addition(ret, a << i);
			for (j = 1; j <= i; j++) {
				// And add the appropriate carries
				ret = GF_addition(ret, ((a >> 8 - j) & 0x01) * carry[i - j]);
			}
		}
	}
	return ret;
}

uint8_t GF_getAddInverse(uint8_t x) {
	// Each element has itself as inverse of addition in GF(2^8)
	return x;
}

uint8_t GF_getMulInverse(uint8_t d) {
	// Notes that it's based on Extended Euclidean Algorithm
	// and subtraction is exactly the same as addition (XOR operation)
	int amt;                                 // Shift amount
	int msb;                                 // Boundary for comparing bit position
	uint8_t r = IRR_POLY;                    // Ramainder
	uint8_t q = 0x00;                        // Quotient
	uint8_t p0 = 0x00, p1 = 0x01, p2;        // Euclidean numbers to get multiplication inverse
	                                         // And parameter d as dividend
	// If dividend is 0, set inverse as 0
	if (d == 0x00) return 0;

	// Deal with x^8 which is appeared at first round
	msb = getMsb(d);
	amt = 8 - msb;
	
	// Calculate quotient and remainder for x^8 first
	q |= 0x01 << amt;
	r = GF_addition(r, d << amt);

	// Continue dividing until degree of remainder is below of dividend
	while (getMsb(r) >= msb) {
		amt = getMsb(r) - msb;
		q |= 0x01 << amt;
		r = GF_addition(r, d << amt);
	}

	// Set the Euclidean number according to the ignition formula below
	// p[i] = p[i - 2] - p[i - 1] * q[i - 2]  
	p2 = GF_addition(p0, GF_multiplication(p1, q));

	// Process remaining rounds (under degree of 7)
	while (r != 0) {
		// Swap dividend and remainder
		swap(&d, &r);

		// Renew before round start
		p0 = p1;
		p1 = p2;
		q = 0;

		// Get boundary
		msb = getMsb(d);
		
		// Divide until degree of remainder is below of dividend
		while (getMsb(r) >= msb) {
			amt = getMsb(r) - msb;
			q |= 0x01 << amt;
			r = GF_addition(r, d << amt);
		}

		// Set the Euclidean number
		p2 = GF_addition(p0, GF_multiplication(p1, q));
	}
	return p1;
}
