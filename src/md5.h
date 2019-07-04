#ifndef _MD5_ENCRYPT_H_H_H
#define _MD5_ENCRYPT_H_H_H

#include <string.h>
#include <string>
using namespace std;

typedef struct
{
	unsigned int state[4];
	unsigned int count[2];
	unsigned char buffer[64];
} MD5Context;

#define S11 7
#define S12 12
#define S13 17
#define S14 22

#define S21 5
#define S22 9
#define S23 14
#define S24 20

#define S31 4
#define S32 11
#define S33 16
#define S34 23

#define S41 6
#define S42 10
#define S43 15
#define S44 21

#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))

#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

#define FF(a, b, c, d, x, s, ac)          \
{ \
(a) += F((b), (c), (d)) + (x)+(unsigned int)(ac); \
(a) = ROTATE_LEFT((a), (s)); \
(a) += (b); \
}

#define GG(a, b, c, d, x, s, ac)	\
{	\
(a) += G((b), (c), (d)) + (x)+(unsigned int)(ac);	\
(a) = ROTATE_LEFT((a), (s)); (a) += (b);	\
}

#define HH(a, b, c, d, x, s, ac)          \
{ \
(a) += H((b), (c), (d)) + (x)+(unsigned int)(ac); \
(a) = ROTATE_LEFT((a), (s)); \
(a) += (b); \
}

#define II(a, b, c, d, x, s, ac)          \
{ \
(a) += I((b), (c), (d)) + (x)+(unsigned int)(ac); \
(a) = ROTATE_LEFT((a), (s)); \
(a) += (b); \
}

static unsigned char PADDING[64] =
{
	0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

const char HEXMAP[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

class MD5Encrypt
{
public:
	MD5Encrypt();
	~MD5Encrypt();

	bool MD5_Encrypt(unsigned char *dst, unsigned char *src, int len);
	string MD5_Encrypt(unsigned char *src, int len);
private:
	void Initialize(MD5Context * context);
	void MD5_Update(MD5Context * context, unsigned char * buf, int len);
	void MD5_Transform(unsigned int state[4], unsigned char block[64]);

	void MD5_Final(MD5Context * context, unsigned char digest[16]);
	void MD5_Encode(unsigned char * output, unsigned int * input, int len);
	void MD5_Decode(unsigned int * output, unsigned char * input, int len);
};

#endif
