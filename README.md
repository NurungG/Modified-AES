## Modified AES 
* Irriducible polynomial on GF(2<sup>8</sup>) and Final vector on Affine transformation is different from standard
* Use irriducible polynomial as x<sup>8</sup>+x<sup>6</sup>+x<sup>3</sup>+x<sup>2</sup>+1
* And final vector is 0x15
* But can be easily adapted by changing ```IRR_POLY``` and ```FINAL_VECTOR```

### 1. Galois Field Operations & Inverse Manipulation
* Addition
* Multiplication

### 2. S-Box & Inverse S-Box Manipulation
* S-Box
* Inverse S-Box

### 3. Round Constant & Key Expansion
* Round constant
* Key Expansion
* R-Function

### 4. Encryption
* Substitute Bytes
* Shift Rows
* Mix Columns
* Add Round Key

### 5. Decryption
* Inverse Subsitute Bytes
* Inverse Shift Rows
* Inverse Mix Columns
