#include "stdafx.h"
#include "AES.h"


#define xtime(x) ((x<<1) ^ (((x>>7) & 1) * 0x1b))

// Multiply is a macro used to multiply numbers in the field GF(2^8)
#define Multiply(x,y) (((y & 1) * x) ^ ((y>>1 & 1) * xtime(x)) ^ ((y>>2 & 1) * xtime(xtime(x))) ^ ((y >> 3 & 1) * xtime(xtime(xtime(x)))) ^ ((y >> 4 & 1) * xtime(xtime(xtime(xtime(x))))))



AES::AES(void)
{
	
}


AES::~AES(void)
{
}

//static unsigned char CipherKey[0x04][0x04] = {0, };

static unsigned char S_BOX[0x10][0x10] = {				// S-BOX
//		 0     1     2     3     4     5     6     7     8     9     a     b     c     d     e     f
	{ 0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76 },	// 0
	{ 0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0 },	// 1
	{ 0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15 },	// 2
	{ 0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75 },	// 3
	{ 0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84 },	// 4
	{ 0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf },	// 5
	{ 0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8 },	// 6
	{ 0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2 },	// 7
	{ 0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73 },	// 8
	{ 0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb },	// 9
	{ 0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79 },	// a
	{ 0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08 },	// b
	{ 0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a },	// c
	{ 0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e },	// d
	{ 0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf },	// e
	{ 0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16 }	// f
};

static unsigned char Inv_S_BOX[0x10][0x10] = {				// S-BOX
//		0     1     2     3     4     5     6     7     8     9     a     b     c     d     e     f
	{ 0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb },	// 0
	{ 0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb },	// 1
	{ 0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e },	// 2
	{ 0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25 },	// 3
	{ 0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92 },	// 4
	{ 0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84 },	// 5
	{ 0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06 },	// 6
	{ 0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b },	// 7
	{ 0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73 },	// 8
	{ 0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e },	// 9
	{ 0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b },	// a
	{ 0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4 },	// b
	{ 0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f },	// c
	{ 0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef },	// d
	{ 0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61 },	// e
	{ 0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d }	// f
};

CString AES::Decrypt(unsigned char state[0x04][0x04]) {
	CString temp;
	/*unsigned char CipherKey[0x04][0x04] = {			// Cipher Key
	{ 0x2b, 0x28, 0xab, 0x09 },
	{ 0x7e, 0xae, 0xf7, 0xcf },
	{ 0x15, 0xd2, 0x15, 0x4f },
	{ 0x16, 0xa6, 0x88, 0x3c }
	};*/

	unsigned char CipherKey[0x04][0x04] = { 0, };
	int i, j = 0;

	Inv_initCipher(CipherKey);
	//���� ����

	//������ ���� Round 10�� CipherKey���� �����Ű�� ����
	for (i = 0; i < 10; i++)
		Inv_KeySchedule(CipherKey, S_BOX, i);
	Inv_AddRoundKey(state, CipherKey);
	//���� �Ϸ�Ǹ� CipherKey �ʱ�ȭ
	Inv_initCipher(CipherKey);

	Inv_ShiftRows(state);
	Inv_SubBytes(state, Inv_S_BOX);

	for (i = 9; i> 0; i--) {
		//�� Round�� ���긶�� �ش� CipherKey �����Ͽ� ������ �� �ʱ�ȭ
		for (j = 0; j < i; j++) {
			Inv_KeySchedule(CipherKey, S_BOX, j);
		}
		Inv_AddRoundKey(state, CipherKey);
		Inv_initCipher(CipherKey);

		Inv_MixColumns(state);
		Inv_ShiftRows(state);
		Inv_SubBytes(state, Inv_S_BOX);
	}

	Inv_AddRoundKey(state, CipherKey);

	//	printf("CipherText\n");
	temp.Format(_T(""));
	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			temp += state[i][j];

	return temp;
}

void AES::Inv_initCipher(unsigned char CipherKey[0x04][0x04])
{
	//srand((unsigned int)time(NULL));

	/*unsigned char temp[0x04][0x04];
	int dec[16] = { 0, };

	int k = 0;
	char* st = LPSTR(LPCTSTR(temp));

	query_state = mysql_query(connection, "SELECT * FROM testkey");

	sql_result = mysql_store_result(connection);

	while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
	{
	for (int i = 0; i < 4; i++)
	for (int j = 0; j < 4; j++)
	temp[i][j] = 0;

	for (int i = 0; i < 16; i++)
	temp_dec[i] = "";


	CString cipherkey;
	cipherkey = sql_row[0];


	char* st = LPSTR(LPCTSTR(cipherkey));


	int m = 0;
	for (int x = 0; x < cipherkey.GetLength(); x++)
	{
	if (st[x] != '!')
	{
	temp_dec[m] += st[x];
	}
	else
	{
	if (x == 0)
	m = 0;
	else
	m++;
	}
	}
	for (int x = 0; x < 16; x++)
	{
	temp_int[x] = _ttoi(temp_dec[x]);
	dec[x] = m_rsa->Decrypt(temp_int[x]);
	}

	}
	*/
	unsigned char tmp[0x04][0x04] = { // Original Cipher Key
		{ 0x2b, 0x28, 0xab, 0x09 },
		{ 0x7e, 0xae, 0xf7, 0xcf },
		{ 0x15, 0xd2, 0x15, 0x4f },
		{ 0x16, 0xa6, 0x88, 0x3c }
	};

	int i, j;
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			CipherKey[i][j] = tmp[i][j];
		}
	}
}

void AES::Inv_SubBytes(unsigned char state[0x04][0x04], unsigned char S_BOX[0x10][0x10]) {
	int i, j, x, y;

	for (i = 0; i<4; i++) {
		for (j = 0; j<4; j++) {
			x = state[i][j] >> 4;
			y = state[i][j] - (x << 4);
			state[i][j] = S_BOX[x][y];
		}
	}
}

void AES::Inv_ShiftRows(unsigned char state[0x04][0x04]) {
	int i, j, k, tmp;

	for (i = 3; i>0; i--) {
		for (j = i - 1; j<3; j++) {
			tmp = state[i][0];
			for (k = 0; k < 3; k++) {
				state[i][k] = state[i][k + 1];
			}
			state[i][3] = tmp;
		}
	}
}

void AES::Inv_MixColumns(unsigned char state[0x04][0x04]) {
	unsigned char a[0x04];
	int i, j;
	for (i = 0; i<4; i++) {
		for (j = 0; j<4; j++) {
			a[j] = state[j][i];
		}
		state[0][i] = 0xFF & (Multiply(a[0], 0x0e) ^ Multiply(a[1], 0x0b) ^ Multiply(a[2], 0x0d) ^ Multiply(a[3], 0x09));
		state[1][i] = 0xFF & (Multiply(a[0], 0x09) ^ Multiply(a[1], 0x0e) ^ Multiply(a[2], 0x0b) ^ Multiply(a[3], 0x0d));
		state[2][i] = 0xFF & (Multiply(a[0], 0x0d) ^ Multiply(a[1], 0x09) ^ Multiply(a[2], 0x0e) ^ Multiply(a[3], 0x0b));
		state[3][i] = 0xFF & (Multiply(a[0], 0x0b) ^ Multiply(a[1], 0x0d) ^ Multiply(a[2], 0x09) ^ Multiply(a[3], 0x0e));
	}
}

// XOR can self inverse-operation.
void AES::Inv_AddRoundKey(unsigned char state[0x04][0x04], unsigned char RoundKey[0x04][0x04]) {
	int i, j;

	for (i = 0; i<4; i++) {
		for (j = 0; j<4; j++) {
			state[i][j] ^= RoundKey[i][j];
		}
	}
}

void AES::Inv_KeySchedule(unsigned char CipherKey[0x04][0x04], unsigned char S_BOX[0x10][0x10], int i) {
	unsigned char Rcon[0x04][0x0a] = {
		{ 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36 },
		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
	};
	unsigned char tmp[0x04][0x04];
	unsigned char tmp2[0x04];
	int j, k, x, y;

	for (j = 0; j<3; j++) {// RotWord
		tmp2[j] = CipherKey[j + 1][3];
	}
	tmp2[3] = CipherKey[0][3];

	for (j = 0; j<4; j++) {// SubBytes
		x = tmp2[j] >> 4;
		y = tmp2[j] - (x << 4);
		tmp2[j] = S_BOX[x][y];
	}

	for (j = 0; j<4; j++) {
		tmp[j][0] = CipherKey[j][0] ^ tmp2[j] ^ Rcon[j][i];
	}

	for (j = 0; j<4; j++) {
		for (k = 1; k<4; k++) {
			tmp[j][k] = CipherKey[j][k] ^ tmp[j][k - 1];
		}
	}

	for (i = 0; i<4; i++) {
		for (j = 0; j<4; j++) {
			CipherKey[i][j] = tmp[i][j];
		}
	}
}



CString AES::Encrypt(unsigned char state[0x04][0x04])
{
	CString temp;
	unsigned char CipherKey[0x04][0x04] = { 0, };

	Inv_initCipher(CipherKey);
	int i, j;

	//���� ����


	AddRoundKey(state, CipherKey); // cipher Ű ����, plantext �� �����Ŵ.

	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			temp += state[i][j];

	for (i = 0; i<9; i++) {
		SubBytes(state, S_BOX);
		ShiftRows(state);
		MixColumns(state);
		KeySchedule(CipherKey, S_BOX, i); //cipherŰ �ٲ�
		AddRoundKey(state, CipherKey); //�ٲ�Ű�� ����
	}

	SubBytes(state, S_BOX);
	ShiftRows(state);
	KeySchedule(CipherKey, S_BOX, i);

	AddRoundKey(state, CipherKey);
	temp.Format(_T(""));
	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			temp += state[i][j];

	return temp;
}

void AES::SubBytes(unsigned char state[0x04][0x04], unsigned char S_BOX[0x10][0x10]) {
	int i, j, x, y;

	for (i = 0; i<4; i++) {
		for (j = 0; j<4; j++) {
			x = state[i][j] >> 4;
			y = state[i][j] - (x << 4);
			state[i][j] = S_BOX[x][y];
		}
	}
}

void AES::ShiftRows(unsigned char state[0x04][0x04]) {
	int i, j, k, tmp;

	for (i = 0; i<4; i++) {
		for (j = 4 - i; j<4; j++) {
			tmp = state[i][0];
			for (k = 0; k<4 - 1; k++) {
				state[i][k] = state[i][k + 1];
			}
			state[i][3] = tmp;
		}
	}
}

void AES::MixColumns(unsigned char state[0x04][0x04]) {
	unsigned char a[0x04] = { 0, }, b[0x04] = { 0, }, h = 0;
	int i, j;

	for (i = 0; i<4; i++) {
		for (j = 0; j<4; j++) {
			a[j] = state[j][i];
			h = (unsigned char)((signed char)state[j][i] >> 7);
			b[j] = state[j][i] << 1;
			b[j] ^= 0x1b & h;
		}
		state[0][i] = b[0] ^ a[3] ^ a[2] ^ b[1] ^ a[1];
		state[1][i] = b[1] ^ a[0] ^ a[3] ^ b[2] ^ a[2];
		state[2][i] = b[2] ^ a[1] ^ a[0] ^ b[3] ^ a[3];
		state[3][i] = b[3] ^ a[2] ^ a[1] ^ b[0] ^ a[0];
	}
}

void AES::AddRoundKey(unsigned char state[0x04][0x04], unsigned char RoundKey[0x04][0x04]) {
	int i, j;

	for (i = 0; i<4; i++) {
		for (j = 0; j<4; j++) {
			state[i][j] ^= RoundKey[i][j];
		}
	}

}

void AES::KeySchedule(unsigned char CipherKey[0x04][0x04], unsigned char S_BOX[0x10][0x10], int i) {
	unsigned char Rcon[0x04][0x0a] = {
		{ 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36 },
		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
	};
	unsigned char tmp[0x04][0x04];
	unsigned char tmp2[0x04];
	int j, k, x, y;

	for (j = 0; j<3; j++) {// RotWord
		tmp2[j] = CipherKey[j + 1][3];
	}
	tmp2[3] = CipherKey[0][3];

	for (j = 0; j<4; j++) {// SubBytes
		x = tmp2[j] >> 4;
		y = tmp2[j] - (x << 4);
		tmp2[j] = S_BOX[x][y];
	}
	// Each round's Wi_1 value
	for (j = 0; j<4; j++) {
		tmp[j][0] = CipherKey[j][0] ^ tmp2[j] ^ Rcon[j][i];
	}
	// Wi_2 ~ Wi_4
	for (j = 0; j<4; j++) {
		for (k = 1; k<4; k++) {
			tmp[j][k] = CipherKey[j][k] ^ tmp[j][k - 1];
		}
	}
	for (i = 0; i<4; i++) {
		for (j = 0; j<4; j++) {
			CipherKey[i][j] = tmp[i][j];
		}
	}
}