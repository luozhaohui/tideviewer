#include "md5.h"
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#pragma warning(disable : 4996)
#define IDU_BUTTON1 100

char szClassName[] = "window";

typedef unsigned char*	POINTER;
typedef unsigned short int UINT2;
typedef unsigned long int UINT4;
typedef struct {
	UINT4 state[4];
	UINT4 count[2];
	unsigned char buffer[64];
} MD5_CTX;

static void MD5Init(MD5_CTX* );
static void MD5Update(MD5_CTX* , unsigned char* , unsigned int);
static void MD5Final(unsigned char[16], MD5_CTX* );
static void MD5Transform(UINT4[4], unsigned char[64]);
static void Encode(unsigned char* , UINT4* , unsigned int);
static void Decode(UINT4* , unsigned char* , unsigned int);
static void MD5_memcpy(POINTER, POINTER, unsigned int);
static void MD5_memset(POINTER, int, unsigned int);

#define S11					7
#define S12					12
#define S13					17
#define S14					22
#define S21					5
#define S22					9
#define S23					14
#define S24					20
#define S31					4
#define S32					11
#define S33					16
#define S34					23
#define S41					6
#define S42					10
#define S43					15
#define S44					21
#define TEST_BLOCK_LEN		1000
#define TEST_BLOCK_COUNT	1000
#define MD5Final			MD5Final
static unsigned char PADDING[64] ={
	0x80,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};
#define F(x, y, z)			(((x) & (y)) | ((~x) & (z)))
#define G(x, y, z)			(((x) & (z)) | ((y) & (~z)))
#define H(x, y, z)			((x) ^ (y) ^ (z))
#define I(x, y, z)			((y) ^ ((x) | (~z)))
#define ROTATE_LEFT(x, n)	(((x) << (n)) | ((x) >> (32 - (n))))
#define FF(a, b, c, d, x, s, ac) {					  \
		(a) += F((b), (c), (d)) + (x) + (UINT4) (ac); \
		(a) = ROTATE_LEFT((a), (s));				  \
		(a) += (b);									  \
	}
#define GG(a, b, c, d, x, s, ac) {					  \
		(a) += G((b), (c), (d)) + (x) + (UINT4) (ac); \
		(a) = ROTATE_LEFT((a), (s));				  \
		(a) += (b);									  \
	}
#define HH(a, b, c, d, x, s, ac) {					  \
		(a) += H((b), (c), (d)) + (x) + (UINT4) (ac); \
		(a) = ROTATE_LEFT((a), (s));				  \
		(a) += (b);									  \
	}
#define II(a, b, c, d, x, s, ac) {					  \
		(a) += I((b), (c), (d)) + (x) + (UINT4) (ac); \
		(a) = ROTATE_LEFT((a), (s));				  \
		(a) += (b);									  \
	}

void MD5Init(MD5_CTX* context)
{
	context->count[0] = context->count[1] = 0;
	context->state[0] = 0x67452301;
	context->state[1] = 0xefcdab89;
	context->state[2] = 0x98badcfe;
	context->state[3] = 0x10325476;
}

void MD5Update(MD5_CTX* context, unsigned char* input, unsigned int inputLen)
{
	unsigned int i;

	unsigned int index;

	unsigned int partLen;
	index = (unsigned int)((context->count[0] >> 3) & 0x3F);
	if ((context->count[0] += ((UINT4) inputLen << 3)) < ((UINT4) inputLen << 3)) {
		context->count[1]++;
	}

	context->count[1] += ((UINT4) inputLen >> 29);
	partLen = 64 - index;
	if (inputLen >= partLen) {
		MD5_memcpy((POINTER) & context->buffer[index], (POINTER) input, partLen);
		MD5Transform(context->state, context->buffer);
		for (i = partLen; i + 63 < inputLen; i += 64) {
			MD5Transform(context->state, &input[i]);
		}

		index = 0;
	}
	else {
		i = 0;
	}

	MD5_memcpy((POINTER) & context->buffer[index], (POINTER) & input[i], inputLen - i);
}

void MD5Final(unsigned char digest[16], MD5_CTX* context)
{
	unsigned char bits[8];
	unsigned int index;
	unsigned int padLen;
	Encode(bits, context->count, 8);
	index = (unsigned int)((context->count[0] >> 3) & 0x3f);
	padLen = (index < 56) ? (56 - index) : (120 - index);
	MD5Update(context, PADDING, padLen);
	MD5Update(context, bits, 8);
	Encode(digest, context->state, 16);
	MD5_memset((POINTER) context, 0, sizeof(*context));
}

static void MD5Transform(UINT4 state[4], unsigned char block[64])
{
	UINT4 a = state[0];

	UINT4 b = state[1];

	UINT4 c = state[2];

	UINT4 d = state[3];

	UINT4 x[16];
	Decode(x, block, 64);
	FF(a, b, c, d, x[0], S11, 0xd76aa478);
	FF(d, a, b, c, x[1], S12, 0xe8c7b756);
	FF(c, d, a, b, x[2], S13, 0x242070db);
	FF(b, c, d, a, x[3], S14, 0xc1bdceee);
	FF(a, b, c, d, x[4], S11, 0xf57c0faf);
	FF(d, a, b, c, x[5], S12, 0x4787c62a);
	FF(c, d, a, b, x[6], S13, 0xa8304613);
	FF(b, c, d, a, x[7], S14, 0xfd469501);
	FF(a, b, c, d, x[8], S11, 0x698098d8);
	FF(d, a, b, c, x[9], S12, 0x8b44f7af);
	FF(c, d, a, b, x[10], S13, 0xffff5bb1);
	FF(b, c, d, a, x[11], S14, 0x895cd7be);
	FF(a, b, c, d, x[12], S11, 0x6b901122);
	FF(d, a, b, c, x[13], S12, 0xfd987193);
	FF(c, d, a, b, x[14], S13, 0xa679438e);
	FF(b, c, d, a, x[15], S14, 0x49b40821);
	GG(a, b, c, d, x[1], S21, 0xf61e2562);
	GG(d, a, b, c, x[6], S22, 0xc040b340);
	GG(c, d, a, b, x[11], S23, 0x265e5a51);
	GG(b, c, d, a, x[0], S24, 0xe9b6c7aa);
	GG(a, b, c, d, x[5], S21, 0xd62f105d);
	GG(d, a, b, c, x[10], S22, 0x2441453);
	GG(c, d, a, b, x[15], S23, 0xd8a1e681);
	GG(b, c, d, a, x[4], S24, 0xe7d3fbc8);
	GG(a, b, c, d, x[9], S21, 0x21e1cde6);
	GG(d, a, b, c, x[14], S22, 0xc33707d6);
	GG(c, d, a, b, x[3], S23, 0xf4d50d87);
	GG(b, c, d, a, x[8], S24, 0x455a14ed);
	GG(a, b, c, d, x[13], S21, 0xa9e3e905);
	GG(d, a, b, c, x[2], S22, 0xfcefa3f8);
	GG(c, d, a, b, x[7], S23, 0x676f02d9);
	GG(b, c, d, a, x[12], S24, 0x8d2a4c8a);
	HH(a, b, c, d, x[5], S31, 0xfffa3942);
	HH(d, a, b, c, x[8], S32, 0x8771f681);
	HH(c, d, a, b, x[11], S33, 0x6d9d6122);
	HH(b, c, d, a, x[14], S34, 0xfde5380c);
	HH(a, b, c, d, x[1], S31, 0xa4beea44);
	HH(d, a, b, c, x[4], S32, 0x4bdecfa9);
	HH(c, d, a, b, x[7], S33, 0xf6bb4b60);
	HH(b, c, d, a, x[10], S34, 0xbebfbc70);
	HH(a, b, c, d, x[13], S31, 0x289b7ec6);
	HH(d, a, b, c, x[0], S32, 0xeaa127fa);
	HH(c, d, a, b, x[3], S33, 0xd4ef3085);
	HH(b, c, d, a, x[6], S34, 0x4881d05);
	HH(a, b, c, d, x[9], S31, 0xd9d4d039);
	HH(d, a, b, c, x[12], S32, 0xe6db99e5);
	HH(c, d, a, b, x[15], S33, 0x1fa27cf8);
	HH(b, c, d, a, x[2], S34, 0xc4ac5665);
	II(a, b, c, d, x[0], S41, 0xf4292244);
	II(d, a, b, c, x[7], S42, 0x432aff97);
	II(c, d, a, b, x[14], S43, 0xab9423a7);
	II(b, c, d, a, x[5], S44, 0xfc93a039);
	II(a, b, c, d, x[12], S41, 0x655b59c3);
	II(d, a, b, c, x[3], S42, 0x8f0ccc92);
	II(c, d, a, b, x[10], S43, 0xffeff47d);
	II(b, c, d, a, x[1], S44, 0x85845dd1);
	II(a, b, c, d, x[8], S41, 0x6fa87e4f);
	II(d, a, b, c, x[15], S42, 0xfe2ce6e0);
	II(c, d, a, b, x[6], S43, 0xa3014314);
	II(b, c, d, a, x[13], S44, 0x4e0811a1);
	II(a, b, c, d, x[4], S41, 0xf7537e82);
	II(d, a, b, c, x[11], S42, 0xbd3af235);
	II(c, d, a, b, x[2], S43, 0x2ad7d2bb);
	II(b, c, d, a, x[9], S44, 0xeb86d391);
	state[0] += a;
	state[1] += b;
	state[2] += c;
	state[3] += d;
	MD5_memset((POINTER) x, 0, sizeof(x));
}

static void Encode(unsigned char* output, UINT4* input, unsigned int len)
{
	unsigned int i;

	unsigned int j;
	for (i = 0, j = 0; j < len; i++, j += 4) {
		output[j] = (unsigned char)(input[i] & 0xff);
		output[j + 1] = (unsigned char)((input[i] >> 8) & 0xff);
		output[j + 2] = (unsigned char)((input[i] >> 16) & 0xff);
		output[j + 3] = (unsigned char)((input[i] >> 24) & 0xff);
	}
}

static void Decode(UINT4* output, unsigned char* input, unsigned int len)
{
	unsigned int i;

	unsigned int j;
	for (i = 0, j = 0; j < len; i++, j += 4) {
		output[i] = ((UINT4) input[j]) | (((UINT4) input[j + 1]) << 8) | (((UINT4) input[j + 2]) << 16) | (((UINT4) input[j + 3]) << 24);
	}
}

static void MD5_memcpy(POINTER output, POINTER input, unsigned int len)
{
	unsigned int i;
	for (i = 0; i < len; i++) {
		output[i] = input[i];
	}
}

static void MD5_memset(POINTER output, int value, unsigned int len)
{
	unsigned int i;
	for (i = 0; i < len; i++) {
		((char*)output)[i] = (char)value;
	}
}

void MD5String(char* dst, const char* src, int srcSize)
{
	MD5_CTX context;
	char str[3];
	unsigned char digest[16];
	MD5Init(&context);
	MD5Update(&context, (unsigned char*)src, (unsigned int)srcSize);
	MD5Final(digest, &context);
	strcpy(dst, "");
	for (int i = 0; i < 16; i++) {
		sprintf(str, "%02x", digest[i]);
		strcat(dst, str);
	}

	strcat(dst, "\0");
}

int MD5File(const char* szFileName)
{
	FILE*  file;
	MD5_CTX context;
	CHAR str[3];
	int len;
	int i;
	unsigned char buffer[1024];
	unsigned char digest[16];
	CHAR md5Str[255];
	strcpy(md5Str, "");

	if ((file = fopen(szFileName, "rb")) == NULL) {
		return 0;
	}
	else {
		MD5Init(&context);
		while (len = (int)fread(buffer, 1, 1024, file)) {
			MD5Update(&context, buffer, (unsigned int)len);
		}

		MD5Final(digest, &context);
		fclose(file);

		for (i = 0; i < 16; i++) {
			sprintf(str, "%02x", digest[i]);
			strcat(md5Str, str);
		}
	}

	strcat(md5Str, "\0");
	md5Str[8] = 0;

	CHAR*  pszEnd;
	int hi = strtoul(md5Str, &pszEnd, 16);

	return hi;
}

#if 0
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static HWND hEdit1;

	static HWND hEdit2;
	CHAR*  str = NULL;
	CHAR *md5 = NULL;
	INT len;
	switch (msg) {
	case WM_CREATE:
		CreateWindow(
			"STATIC",
			"Input string",
			WS_CHILD | WS_VISIBLE,
			10,
			10,
			80,
			28,
			hWnd,
			NULL,
			((LPCREATESTRUCT) lParam)->hInstance,
			NULL);
		hEdit1 = CreateWindowEx(
				WS_EX_CLIENTEDGE,
				"EDIT",
				NULL,
				WS_CHILD | WS_VISIBLE,
				100,
				10,
				180,
				28,
				hWnd,
				NULL,
				((LPCREATESTRUCT) lParam)->hInstance,
				NULL);

		CreateWindow(
			"BUTTON",
			"Compute MD5",
			WS_CHILD | WS_VISIBLE,
			300,
			8,
			150,
			28,
			hWnd,
			(HMENU) IDU_BUTTON1,
			((LPCREATESTRUCT) lParam)->hInstance,
			NULL);

		CreateWindow(
			"STATIC",
			"MD5 value",
			WS_CHILD | WS_VISIBLE,
			10,
			50,
			80,
			28,
			hWnd,
			NULL,
			((LPCREATESTRUCT) lParam)->hInstance,
			NULL);

		hEdit2 = CreateWindowEx(
				WS_EX_CLIENTEDGE,
				"EDIT",
				NULL,
				WS_CHILD | WS_VISIBLE,
				100,
				50,
				350,
				28,
				hWnd,
				NULL,
				((LPCREATESTRUCT) lParam)->hInstance,
				NULL);
		break;

	case WM_GETMINMAXINFO: {
			MINMAXINFO*	 lpMMI = (MINMAXINFO*)lParam;
			lpMMI->ptMinTrackSize.x = 470;
			lpMMI->ptMinTrackSize.y = 150;
		}
		break;
	case WM_COMMAND:

		switch (LOWORD(wParam)) {
		case IDU_BUTTON1: {
				len = GetWindowTextLength(hEdit1) + 1;
				str = (char*)malloc(len);
				GetWindowText(hEdit1, str, len);

				int md5len = 33;
				md5 = (char*)malloc(md5len);
				MD5String(md5, str, len);

				SetWindowText(hEdit2, md5);

				free(md5);
				md5 = NULL;

				free(str);
				str = NULL;
			}
			break;
		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return(DefWindowProc(hWnd, msg, wParam, lParam));
	}

	return(0L);
}

int WINAPI WinMain(HINSTANCE hinst, HINSTANCE hPreInst, LPSTR pCmdLine, int nCmdShow)
{
	HWND hWnd;
	MSG msg;
	WNDCLASS wndclass;
	if (!hPreInst) {
		wndclass.style = CS_HREDRAW | CS_VREDRAW;
		wndclass.lpfnWndProc = WndProc;
		wndclass.cbClsExtra = 0;
		wndclass.cbWndExtra = 0;
		wndclass.hInstance = hinst;
		wndclass.hIcon = NULL;
		wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
		wndclass.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
		wndclass.lpszMenuName = NULL;
		wndclass.lpszClassName = szClassName;
		if (!RegisterClass(&wndclass)) {
			return FALSE;
		}
	}

	hWnd = CreateWindow(
			szClassName,
			"Compute MD5 from string.",
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			470,
			150,
			NULL,
			NULL,
			hinst,
			NULL);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return int(msg.wParam);
}
#endif