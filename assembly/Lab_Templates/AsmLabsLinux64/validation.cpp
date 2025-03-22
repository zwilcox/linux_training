#include <string.h>
#include "validation.h"


#define LAB_VALIDATED(level,num) \
    printf("Lab %d.%d validated (%lu bytes)\n", level, num, lab##level##_##num##_size); \
    submit(SERVER_IP, PORT, YOURNAME, level, num, lab##level##_##num##_size);
#define LAB_VALIDATEDB(level,num,bonusmsg) \
    printf("Lab %d.%d validated (%lu bytes) %s\n", level, num, lab##level##_##num##_size, bonusmsg);\
    submit(SERVER_IP, PORT, YOURNAME, level, num, lab##level##_##num##_size);

struct user_regs_struct regs;
#define run_check(sym, cond) {\
    save_n_go(sym, &regs);\
    if (!(cond)) {\
        printf(#sym " failed\n");\
        exit(-1);\
    }\
}

#define A 0x0100010001000100
#define B 0x0200020002000200
#define C 0x0300030003000300
#define D 0x0400040004000400

extern "C" {
    long long regRAX, regRBX, regRCX, regRDX, regR8, regR9, moveToStack;
    void (*labFunc)();
}

void setRegs() {
    regs.rax = A;
    regs.rbx = B;
    regs.rcx = C;
    regs.rdx = D;
}

void failLab(const char* fmt, ...)
{
    char szData[4096] = "";
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(szData, sizeof(szData), fmt, ap);
    printf("%s\nHit a key to exit.\n", szData);
    va_end(ap);
    system("pause");
    exit(-1);
}

void validate_lab1_1()
{
    regs.rax = 0x1;
    regs.rbx = 0x2;
    run_check(lab1_1, regs.rax == 2 && regs.rbx == 1);
    LAB_VALIDATED(1,1);
}

void validate_lab1_2()
{
    setRegs();
    run_check(lab1_2, regs.rax == (B + C + 0x42) &&
                      regs.rbx == B &&
                      regs.rcx == C &&
                      regs.rdx == D);
    LAB_VALIDATED(1, 2);
}

void validate_lab1_3()
{
    setRegs();
    run_check(lab1_3, regs.rax == B &&
        regs.rbx == A &&
        regs.rcx == C &&
        regs.rdx == D);
    LAB_VALIDATED(1, 3);
}

void validate_lab2_1()
{
    uint8_t* bp = (uint8_t*)lab2_1;

    if (0xCC == *(uint8_t*)lab2_1)
    {
        bp++;
    }

    uint8_t longJmp[] = { 0xE9, 0xfb, 0xff, 0xff, 0xff };
    uint8_t shortJmp[] = { 0xeb, 0xfe };
    if (0 != memcmp(longJmp, bp, sizeof(longJmp)) &&
        0 != memcmp(shortJmp, bp, sizeof(shortJmp)))
    {
        printf("Lab 2.1 failed!\n"
            "You do not appear to have a single jmp infinite loop.\n"
            "Perhaps you ahve extra instructions (e.g. int3) in front of your jmp?\n");
        system("pause");
        exit(-1);
    }
    LAB_VALIDATED(2, 1);
}

void validate_lab2_2()
{
    setRegs();
    regs.rax = 42;
    run_check(lab2_2, regs.rax == 42 && regs.rbx == 1 && regs.rcx == C && regs.rdx == 2);
    LAB_VALIDATED(2, 2);
}

void validate_lab2_3()
{
    setRegs();
    regs.rax = regs.rcx = 1005;
    run_check(lab2_3, regs.rax == 1005 && regs.rbx == B && regs.rcx == 1005 && regs.rdx == D);
    LAB_VALIDATED(2, 3);
}

void validate_lab2_4()
{
    setRegs();
    regs.rax = 1005;
    run_check(lab2_4, regs.rax == 16983 && regs.rbx == B && regs.rdx == D);
    LAB_VALIDATED(2, 4);
}

void validate_lab3_1()
{
    setRegs();
    regs.rax = 0xfedcba9876543210;
    run_check(lab3_1, regs.rax == 0xfedcba9876541032 && regs.rbx == B && regs.rcx == C && regs.rdx == D);
    LAB_VALIDATED(3, 1);
}

void validate_lab3_2()
{
    setRegs();
    regs.rax = 0xfedcba9876543210;
    run_check(lab3_2, regs.rax == 0xfedcba9876543210 && regs.rbx == B && regs.rcx == C && (regs.rdx & 0xff) == 0xfe);
    LAB_VALIDATED(3, 2);
}

void validate_lab3_3()
{
    setRegs();
    regs.rax = 0xfedcba9876543210;
    run_check(lab3_3, regs.rax == 0x3210 && regs.rbx == B && regs.rcx == C && regs.rdx == D);
    LAB_VALIDATED(3, 3);
}

void validate_lab3_4()
{
    setRegs();
    regs.rax = 0x100000;
    run_check(lab3_4, regs.rax == (0x100000 / 64) && regs.rbx == B && regs.rcx == C && regs.rdx == D);
    LAB_VALIDATED(3, 4);
}

void validate_lab3_5()
{
    setRegs();
    regs.rax = 0xfedcba9876543210;
    run_check(lab3_5, regs.rax == 0x1032547698badcfe && regs.rbx == B && regs.rdx == D);
    LAB_VALIDATED(3, 5);
}

size_t tmp;
size_t *pTmp = &tmp;

void validate_lab4_1()
{
    setRegs();
    regs.rcx = (size_t)pTmp;
    if (0x48 == *(uint8_t*)lab4_1) // Check for the REX prefix
    {
        if (0x93 == *((uint8_t*)(lab4_1)+1))
        {
            failLab("Lab 4.1 failed! You are not supposed to use xchg!\n");
        }
    }
    else if (0x93 == *(uint8_t*)(lab4_1))
    {
        failLab("Lab 4.1 failed! You are not supposed to use xchg!\n");
    }

    run_check(lab4_1, regs.rax == B && regs.rbx == A && regs.rcx == (size_t)pTmp && regs.rdx == D);
    LAB_VALIDATED(4, 1);
}

void validate_lab4_2()
{
    setRegs();
    size_t ma = 0, mb = 0x0123456789abcdef;
    size_t* pma = &ma;
    size_t* pmb = &mb;
    regs.rax = (size_t)pma;
    regs.rbx = (size_t)pmb;
    run_check(lab4_2, regs.rax == (size_t)pma && regs.rcx == C && regs.rdx == D);
    if (mb != 0x0123456789abcdef)
    {
        failLab("Lab 4.2 failed! You modified memory pointed to be RBX!\n");
    }
    if (0xef != ma)
    {
        failLab("Lab 4.2 failed! DWORD pointed to by RAX is 0x%16llX, should be 0x00000000000000ef!\n", ma);
    }
    LAB_VALIDATED(4, 2);
}

void validate_lab4_3()
{
    setRegs();
    run_check(lab4_3, regs.rax == (B + C) && regs.rbx == B && regs.rcx == C && regs.rdx == D);
    LAB_VALIDATED(4, 3);
}

void validate_lab4_4()
{
    uint8_t buffer[1024] = { 0 };
    uint8_t* pb = buffer;
    memset(buffer, 0xcc, sizeof(buffer));
    setRegs();
    regs.rax = (size_t)pb;
    run_check(lab4_4, regs.rax == (size_t)buffer && regs.rbx == B && regs.rdx == D);
    unsigned int i;
    for (i = 0; i < sizeof(buffer); i++)
    {
        if (i <= 64)
        {
            if (buffer[i] != i)
            {
                failLab("Lab 4.4 failed!\n"
                    "Buffer pointed to by EAX+%d should be %d (0x%02X) but is %d (0x%02X)%s\n",
                    i, i, i, buffer[i], buffer[i],
                    buffer[i] == 0xCC ? " (probably uninitialized)" : "");
            }
        }
        else if (0xCC != buffer[i])
        {
            failLab("Lab 4.4 failed! You wrote past the buffer!\n");
        }
    }


    LAB_VALIDATED(4, 4);
}

void validate_lab4_5_string(const char *sz)
{
    char orig[1024] = { 0 };
    char buffer[1024] = { 0 };
    char* pb = buffer;

    memset(buffer, 0xCC, sizeof(buffer));
    strncpy(orig, sz, sizeof(buffer));

    regs.rax = (size_t)sz;
    regs.rbx = (size_t)pb;

    run_check(lab4_5, regs.rax == (size_t)sz && regs.rbx == (size_t)pb);

    size_t actualLen = strlen(sz);
    size_t givenLen = (size_t)(*(uint16_t*)buffer);

    if (*(uint16_t*)buffer == 0xCCCC)
    {
        failLab("Lab 4.5 failed! You did not store the string's length!\n");
    }

    if (givenLen != actualLen)
    {
        failLab("Lab 4.5 failed! You reported string length as %d(0x%02X), it's actually %d(0x%02X)\n",
            givenLen, givenLen, actualLen, actualLen);
    }

    if (0 != memcmp(buffer + 2, sz, actualLen))
    {
        failLab("Lab 4.5 failed! Strings do not match!\n");
    }

    if (!buffer[2 + actualLen])
    {
        failLab("Lab 4.5 failed! You copied the NULL!\n");
    }

    if (0 != strcmp(orig, sz))
    {
        failLab("Lab 4.5 failed! You modified the original string!\n");
    }
}

void validate_lab4_5()
{
    validate_lab4_5_string("Hello");
    validate_lab4_5_string("");
    validate_lab4_5_string("G");
    validate_lab4_5_string("Little man quit poking me");
    LAB_VALIDATED(4, 5);
}

void validate_lab5_1()
{
    setRegs();
    run_check(lab5_1, regs.rax == B && regs.rbx == A && regs.rcx == C && regs.rdx == D);
    LAB_VALIDATED(5, 1);
}

void validate_lab5_2()
{
    setRegs();
    modify_stack_lab5_2();
    LAB_VALIDATED(5, 2);
}

void validate_lab5_3_string(const char* sz)
{
    uint8_t buffer[1024];
    memset(buffer, 0xCC, sizeof(buffer));
    size_t pb = (size_t)buffer;

    setRegs();
    regs.rax = (size_t)sz;
    regs.rbx = (size_t)pb;

    size_t actualLen = strlen(sz);

    run_check(lab5_3, regs.rax == (size_t)actualLen && regs.rbx == pb && regs.rcx == C && regs.rdx == D);

    if (actualLen && 0 != memcmp(sz, buffer, actualLen + 1))
    {
        failLab("Lab 5.3 failed! Didn't copy string right!\n");
    }
    if (*((uint8_t*)buffer + actualLen + 2) != 0xcc)
    {
        failLab("Lab 5.3 failed! You wrote past the end of the string!\n");
    }
}

void validate_lab5_3()
{
    validate_lab5_3_string("Hello");
    validate_lab5_3_string("");
    validate_lab5_3_string("G");
    validate_lab5_3_string("Little man quit poking");
    LAB_VALIDATED(5, 3);
}

void validate_lab6_1_string(const char* sz)
{
    setRegs();
    regs.rax = (size_t)sz;
    size_t actualLen = strlen(sz);
    size_t durp = ((size_t (*)(const char*))(lab6_1))(sz);
    if (durp != actualLen)
    {
        failLab("Lab 6.1 failed\n");
    }
}

void validate_lab6_1()
{
    validate_lab6_1_string("Hello");
    validate_lab6_1_string("");
    validate_lab6_1_string("G");
    validate_lab6_1_string("Hello");
    LAB_VALIDATED(6, 1);
}

void validate_lab6_2_inner(int a, int b, int c, int d, int e, int f, int g)
{
    int result = ((int (*)(int, int, int, int, int, int, int))(lab6_2))(a, b, c, d, e, f, g);
    if (result != (a + b + c + d + e + f + g))
    {
        failLab("Failed lab6.2\n");
    }
}

void validate_lab6_2()
{
    validate_lab6_2_inner(0, 0, 0, 0, 0, 0, 0);
    validate_lab6_2_inner(1, 2, 3, 4, 5, 6, 7);
    validate_lab6_2_inner(0x0, 0x10, 0x100, 0x1000, 0x10000, 0x100000, 0x1000000);
    LAB_VALIDATED(6, 2);
}

extern "C" size_t my_strlen(char* sz)
{
    size_t len = strlen(sz);
    sz = (char*)0xBBBBBBBBBBBBBBBB; //Make sure they don't assume args are re-usable
    return len;
}

void* g_my_malloc_ret = NULL;

extern "C" void* my_malloc(int size) //allocates heap buffer of specified size
{
    if (size <= 0)
    {
        failLab("Lab 6.3 failed! You tried to allocate %d bytes from _my_malloc!\n", size);
    }

    g_my_malloc_ret = malloc(size);
    if (NULL == g_my_malloc_ret)
    {
        failLab("Malloc failed. See the instructor\n");
    }

    memset(g_my_malloc_ret, 0xCC, size);
    size = (int)0xBBBBBBBBBBBBBBBB;  //Make sure they don't assume args are re-usable
    return g_my_malloc_ret;
}

extern "C" void my_memcpy(void* dst, void* src, int size) //copies memory
{
    memcpy(dst, src, size);

    //Make sure they don't assume args are re-usable
    //dst = (void*)0xBBBBBBBBBBBBBBBB;
    //src = (void*)0xBBBBBBBBBBBBBBBB;
    //size = (int)0xBBBBBBBBBBBBBBBB;

}
void validate_lab6_3_string(const char* sz)
{
    size_t actualLen = strlen(sz);
    char * szCopy = ((char * (*)(const char*))(lab6_3))(sz);

    if (g_my_malloc_ret != szCopy)
    {
        failLab("Lab 6.3 failed! You did not return the newly allocated string!\n");
    }

    if (szCopy[actualLen] != 0)
    {
        failLab("Lab 6.3 failed! Did you forget to copy the NULL terminator?\n");
    }

    if (0 != strcmp(szCopy, sz))
    {
        failLab("Lab 6.3 failed! \n  Strings do not match!\n");
    }

    free(szCopy); //free returned string
}

void validate_lab6_3()
{
    validate_lab6_3_string("Hello");
    validate_lab6_3_string("");
    validate_lab6_3_string("G");
    validate_lab6_3_string("Hello");
    LAB_VALIDATED(6, 3);
}

void validate_lab6_4()
{
	unsigned long result = ((unsigned long(*)(unsigned long, ...))(lab6_4))(0);
	if (result != 0)
	{
		failLab("Lab 6.4 failed\n");
	}

	result = ((unsigned long(*)(unsigned long, ...))(lab6_4))(1, 5);
	if (result != 5)
	{
		failLab("Lab 6.4 failed\n");
	}

	result = ((unsigned long(*)(unsigned long, ...))(lab6_4))(2, 10, 13);
	if (result != 23)
	{
		failLab("Lab 6.4 failed\n");
	}

	result = ((unsigned long(*)(unsigned long, ...))(lab6_4))(5, 1, 2, 3, 4, 5);
	if (result != 15)
	{
		failLab("Lab 6.4 failed\n");
	}

	result = ((unsigned long(*)(unsigned long, ...))(lab6_4))(10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10);
	if (result != 100)
	{
		failLab("Lab 6.4 failed\n");
	}

	LAB_VALIDATED(6, 4);
}

void validate_lab6_5()
{
	lab6_5();
	LAB_VALIDATED(6, 5);
}

void validate_lab7_1()
{
    setRegs();
    unsigned long result = ((unsigned long (*)(unsigned long, unsigned long, unsigned long, unsigned long, unsigned long, unsigned long, unsigned long))(lab7_1))(0x44, 0x33, 0x22, 0x11, 0x01, 0x123, 0x456);
    if (result != (0x44 + 0x33 + 0x22 + 0x11 + 0x01 + 0x123 + 0x456))
    {
        failLab("Lab 7.1 failed\n");
    }
    LAB_VALIDATED(7, 1);
}

void validate_lab7_2_string(const char *szA, const char *szB, int maxLength, int res )
{
    setRegs();
    int result = ((int (*)(const char *, const char *, int))(lab7_2))(szA, szB, maxLength);

	if( result != res )
		failLab("Lab 7.2 failed!\n"
				"   szA: '%s'\n"
				"   szB: '%s'\n"
				"   maxLength: %d\n"
				" Expected Result: %d\n"
				" Your Result:     %d\n",
				szA, szB, maxLength, res, result );
}

void validate_lab7_2()
{
	validate_lab7_2_string("","",0,0);
	validate_lab7_2_string("","",1,0);
	validate_lab7_2_string("X","",1,1);
	validate_lab7_2_string("X","Y",1,1);
	validate_lab7_2_string("X","Y",0,0);
	validate_lab7_2_string("XX","XY",1,0);
	validate_lab7_2_string("XX","XY",2,1);
	validate_lab7_2_string("XX","XXX",2,0);
	validate_lab7_2_string("XX","XXX",3,1);
	validate_lab7_2_string("XX","XXX",2000,1);
	validate_lab7_2_string("XXX\0Y","XXX\0X",2000,0);
	validate_lab7_2_string("Hello", "Hello, World!", 5, 0);
	validate_lab7_2_string("Hello", "Hello, World!", 6, 1);
	validate_lab7_2_string("Hello, World!", "Hello", 5, 0);
	validate_lab7_2_string("Hello, World!", "Hello", 6, 1);
	LAB_VALIDATED(7, 2);
}

void validate_lab7_3_string(const char *sz)
{
    //char *_stdcall copyString(const char *sz);
	char * res = ((char * (*)(const char *))(lab7_3))(sz);
	int actualLen = strlen(sz);
	char *szCopy = (char *)res;

	if( g_my_malloc_ret != szCopy )
		failLab("Lab 7.3 failed! You did not return the newly allocated string!\n");

	if( szCopy[actualLen]!=0)
		failLab("Lab 7.3 failed! Did you forget to copy the NULL terminator?\n");

	if( 0!=strcmp(szCopy, sz) )
		failLab("Lab 7.3 failed! \n"
			    "  Strings do not match!\n");

	free(szCopy); //free returned string
}

void validate_lab7_3()
{
	validate_lab7_3_string("Hello");
	validate_lab7_3_string("");
	validate_lab7_3_string("G");
	validate_lab7_3_string("Hello");
	LAB_VALIDATED(7, 3 );
}

void validate_lab8_1_inner(const char* sz)
{
    uint8_t buffer[500];
    uint8_t* pbuf = buffer;
    memset(buffer, 0xCC, sizeof(buffer));

    setRegs();
    size_t len = strlen(sz);
    ((void (*)(void*, const char*, size_t))(lab8_1))(pbuf, sz, len);
    if (0 != memcmp(buffer, sz, len))
    {
        failLab("Lab 8.1 failed\nMemory not copied correctly\n");
    }

    if (buffer[len] != 0xCC)
    {
        failLab("Lab 8.1 failed\nYou copied too much! Overflow\n");
    }
}

void validate_lab8_1()
{
    validate_lab8_1_inner("");
    validate_lab8_1_inner("X");
    validate_lab8_1_inner("Hello There");
    LAB_VALIDATED(8, 1);
}

void validate_lab8_2_inner(const char* sz)
{
    uint8_t buffer[500];
    uint8_t* pbuf = buffer;
    memset(buffer, 0xCC, sizeof(buffer));

    setRegs();
    size_t len = strlen(sz);
    ((void (*)(void*, const char*, size_t))(lab8_2))(pbuf, sz, len);
    if (0 != memcmp(buffer, sz, len))
    {
        failLab("Lab 8.2 failed\nMemory not copied correctly\n");
    }

    if (buffer[len] != 0xCC)
    {
        failLab("Lab 8.2 failed\nYou copied too much! Overflow\n");
    }
}

void validate_lab8_2()
{
    validate_lab8_2_inner("");
    validate_lab8_2_inner("X");
    validate_lab8_2_inner("Hello There");
    LAB_VALIDATED(8, 2);
}

void validate_lab8_3_inner(int size, int ch)
{
    uint8_t buffer[5000];
    uint8_t* pbuf = buffer;
    memset(buffer, ch + 1, sizeof(buffer));

    ((void (*)(void*, int, int))(lab8_3))(pbuf, ch, size);

    int i;
    for (i = 0; i < size && buffer[i] == ch; i++) {}

    if (i != size)
    {
        failLab("Lab 8.3 failed!\n"
            "  Buffer not filled with %d bytes of 0x%02X! (starting at byte %d)\n",
            size, ch, i);
    }
    if (buffer[size] != ch + 1)
    {
        failLab("Lab 8.3 failed!\n You overwrote the buffer's end!\n");
    }
}

void validate_lab8_3()
{
    validate_lab8_3_inner(0, 0x42);
    validate_lab8_3_inner(1, 0x42);
    validate_lab8_3_inner(2, 0x42);
    validate_lab8_3_inner(600, 0x42);
    LAB_VALIDATED(8, 3);
}

void validate_lab8_4_inner(const char* sz, size_t offset)
{
    uint8_t buffer[5000];
    uint8_t* sbuf = buffer;
    memset(buffer, 0xCC, sizeof(buffer));
    size_t len = strlen(sz) + 1;
    memcpy(buffer, sz, len);
    uint8_t* dbuf = sbuf + offset;

    //void _fastcall slideUp(void *dst, void *src, int size);
    ((void (*)(void*, void*, size_t))(lab8_4))(dbuf, sbuf, len);

    if (0 != memcmp(dbuf, sz, len))
    {
        failLab("Lab 8.4 failed! \n"
            "  Memory not copied correctly!\n");
    }
}

void validate_lab8_4()
{
    validate_lab8_4_inner("", 0);
    validate_lab8_4_inner("X", 0);
    const char* sz = "Hello There";
    validate_lab8_4_inner(sz, 1);
    validate_lab8_4_inner(sz, 2);
    validate_lab8_4_inner(sz, strlen(sz));
    validate_lab8_4_inner(sz, strlen(sz) + 1);
    validate_lab8_4_inner(sz, strlen(sz) + 2);
    LAB_VALIDATED(8, 4);
}

void validate_lab8_5_string(const char* szA, const char* szB, int maxLength, int res)
{
    // int _stdcall my_strncmp(const char *szA, const char *szB, int maxLength);
    int result = ((int (*)(const char*, const char*, int))(lab8_5))(szA, szB, maxLength);

    if (result != res)
    {
        failLab("Lab 8.5 failed!\n"
            "   szA: '%s'\n"
            "   szB: '%s'\n"
            "   maxLength: %d\n"
            " Expected Result: %d\n"
            " Your Result:     %d\n",
            szA, szB, maxLength, res, result);
    }
}

void validate_lab8_5()
{
    validate_lab8_5_string("", "", 0, 0);
    validate_lab8_5_string("", "", 1, 0);
    validate_lab8_5_string("X", "", 1, 1);
    validate_lab8_5_string("X", "Y", 1, 1);
    validate_lab8_5_string("X", "Y", 0, 0);
    validate_lab8_5_string("XX", "XY", 1, 0);
    validate_lab8_5_string("XX", "XY", 2, 1);
    validate_lab8_5_string("XX", "XXX", 2, 0);
    validate_lab8_5_string("XX", "XXX", 3, 1);
    validate_lab8_5_string("XX", "XXX", 2000, 1);
    validate_lab8_5_string("XXX\0Y", "XXX\0X", 2000, 0);
    LAB_VALIDATED(8, 5);
}

void validate_lab8_6_string(const char* sz, int ch)
{
    size_t len = strlen(sz);
    //const char *_stdcall my_memchr(void *buf, char ch, int count);
    size_t result = (size_t)((const char * (*)(const char*, char, size_t))(lab8_6))(sz, ch, len);
    size_t res = (size_t)memchr(sz, ch, len);

    if (result != res)
        failLab("Lab 8.6 failed!\n"
            "   sz:   '%s'\n"
            "   ch:   '%c'\n"
            "   count: %d\n"
            " Expected Result: %p\n"
            " Your Result:     %p\n",
            sz, ch, len, res, result);
}

void validate_lab8_6()
{
    validate_lab8_6_string("", 'X');
    validate_lab8_6_string("XY", 'X');
    validate_lab8_6_string("YX", 'X');
    validate_lab8_6_string("YX", 'Z');
    validate_lab8_6_string("Hello", 'l');
    LAB_VALIDATEDB(8, 6, "(BONUS)");
}

void validate_lab9_1()
{
    int result = ((int (*)())(val_lab9_1))();
    if (result != 1)
    {
        failLab("Lab 9.1 failed! ZF not set!\n");
    }
    LAB_VALIDATED(9, 1);
}

void validate_lab9_2()
{
    int result = ((int (*)(struct user_regs_struct*))(val_lab9_2))(&regs);

    if (result != 1 || regs.rbx != 1)
    {
        failLab("Lab 9.2 failed! Both CF and OF are not set!\n");
    }
    LAB_VALIDATED(9, 2);
}


void validate_lab10_1()
{
    unsigned long result = ((int (*)(unsigned long, unsigned long))(lab10_1))(100, 3);
    if (result != 33)
    {
        failLab("Lab 10.1 failed! Wrong answer!\n");
    }

    result = ((int (*)(unsigned long, unsigned long))(lab10_1))(5, 5);
    if (result != 1)
    {
        failLab("Lab 10.1 failed! Wrong answer!\n");
    }

    result = ((int (*)(unsigned long, unsigned long))(lab10_1))(137493700, 737);
    if (result != 137493700 / 737)
    {
        failLab("Lab 10.1 failed! Wrong answer!\n");
    }
    LAB_VALIDATED(10, 1);
}

void validate_lab10_2()
{
    regs.rcx = 0xff;
    run_check(lab10_2, regs.rax == 3 && regs.rdx == 2 && regs.rcx == 0xf0);

    regs.rcx = 0x00;
    run_check(lab10_2, regs.rax == 3 && regs.rdx == 0 && regs.rcx == 0x03);

    regs.rcx = 0x65;
    run_check(lab10_2, regs.rax == 6 && regs.rdx == 2 && regs.rcx == 0x62);

    LAB_VALIDATED(10, 2);
}

void validate_lab11_1()
{
    setRegs();
    regs.rdi = 1;
    run_check(lab11_1, regs.rax == 0);

    regs.rdi = 2;
    run_check(lab11_1, regs.rax == 1);

    regs.rdi = 10;
    run_check(lab11_1, regs.rax == 34);

    regs.rdi = 0x1e; 
    run_check(lab11_1, regs.rax == 514229);

    LAB_VALIDATED(11, 1);
}

void validate_lab11_2()
{
    setRegs();
    regs.rdi = 0x0;
    run_check(lab11_2, regs.rax == 0);

    regs.rdi = 100;
    run_check(lab11_2, regs.rax == 315);

    regs.rdi = 200;
    run_check(lab11_2, regs.rax == 1365);

    regs.rdi = 0xffff;
    run_check(lab11_2, regs.rax == 143193975);

    LAB_VALIDATED(11, 2);
}

void validate_lab11_3()
{
    int list1[] = { 1,2,3,4,5,6,5,7,8,9,1,2 };
    int list2[] = { 1,2,3,4,5 };
    int list3[] = { -10,3,4,5,-20,1,2,3,4,5 };    

    int result = ((int (*)(unsigned int, int*))(lab11_3))(0, 0);
    if (result != -1) {
         failLab("Lab 11.3 failed!\n");   
    }

    result = ((int (*)(unsigned int, int*))(lab11_3))(12, list1);
    if (result != 15120) {
         failLab("Lab 11.3 failed!\n");   
    }

    result = ((int (*)(unsigned int, int*))(lab11_3))(5, list2);
    if (result != 120) {
         failLab("Lab 11.3 failed!\n");   
    }

    result = ((int (*)(unsigned int, int*))(lab11_3))(10, list3);
    if (result != 12000) {
         failLab("Lab 11.3 failed!\n");   
    }

    LAB_VALIDATED(11, 3);
}

void validate_lab11_4()
{
    char enc_xor_str1[] = "hello world";
    char dec_xor_str1[] = { 0x29, 0x24, 0x2d, 0x2d, 0x2e, 0x61, 0x36, 0x2e, 0x33, 0x2d, 0x25, 0x00};
    char enc_xor_str2[] = "test string2";
    char dec_xor_str2[] = { 0x16, 0x07, 0x11, 0x16, 0x42, 0x11, 0x16, 0x10, 0x0b, 0x0c, 0x05, 0x50, 0x00 };
    ((void (*)(char *, unsigned char, unsigned long))lab11_4)(enc_xor_str1, 'A', strlen(enc_xor_str1));
    if (strcmp(enc_xor_str1, dec_xor_str1)) {
        failLab("Lab 11.4 failed!\n");
    }
    
    ((void (*)(char *, unsigned char, unsigned long))lab11_4)(enc_xor_str1, 'A', strlen(enc_xor_str1));
    if (strcmp(enc_xor_str1, "hello world")) {
        failLab("Lab 11.4 failed!\n");
    }

    ((void (*)(char *, unsigned char, unsigned long))lab11_4)(enc_xor_str2, 'b', strlen(enc_xor_str2));
    if (strcmp(enc_xor_str2, dec_xor_str2)) {
        failLab("Lab 11.4 failed!\n");
    }
    
    ((void (*)(char *, unsigned char, unsigned long))lab11_4)(enc_xor_str2, 'b', strlen(enc_xor_str2));
    if (strcmp(enc_xor_str2, "test string2")) {
        failLab("Lab 11.4 failed!\n");
    }
    LAB_VALIDATED(11,4);
}


void validate_lab11_5()
{
    char test_str1[] = "1234567";
    char test_str2[] = "123";
    char test_str3[] = "77777777777777777";

    char* newString = ((char* (*)(char *, unsigned long))lab11_5)(test_str1, strlen(test_str1));
    if (strcmp(newString, "1,234,567")) {
	failLab("Lab 11.5 failed!\n");
    }
    free(newString);

    char* newString2 = ((char* (*)(char *, unsigned long))lab11_5)(test_str2, strlen(test_str2));
    if (strcmp(newString2, "123")) {
	failLab("Lab 11.5 failed!\n");
    }
    free(newString2);

    char *newString3 = ((char* (*)(char *, unsigned long))lab11_5)(test_str3, strlen(test_str3));
    if (strcmp(newString3, "77,777,777,777,777,777")) {
	failLab("Lab 11.5 failed!\n");
    }
    free(newString3);

    LAB_VALIDATED(11,5);
}

void validate_lab11_6()
{
    setRegs();
    regs.rdi = 1;
    run_check(lab11_6, regs.rax == 0);

    regs.rdi = 2;
    run_check(lab11_6, regs.rax == 0);

    regs.rdi = 5;
    run_check(lab11_6, regs.rax == 2);

    regs.rdi = 10;
    run_check(lab11_6, regs.rax == 44);

    regs.rdi = 30; 
    run_check(lab11_6, regs.rax == 257114);

    regs.rdi = 33; 
    run_check(lab11_6, regs.rax == 1089154);

    LAB_VALIDATED(11, 6);
}

void validate_lab11_7()
{
    const char* valid_str[] = { "p@ssWord123", "Passw0rd!", "Passw0)rd", "P%assw0rd" };
    const char* invalid_str[] = { "1password", "password123", "p@ssword123", "P\"ASSWORD", "P\'assword", "1P@ssword" };
    
    int valid_len = 4;
    int invalid_len = 6;   
    int i = 0, result;
    
    for (i = 0; i < invalid_len; i++){
        result = ((int(*)(const char *, unsigned long))lab11_7)(invalid_str[i], strlen(invalid_str[i]));
        if (result) {
	    failLab("Lab 11.7 failed!");
        }
    }

    for (i = 0; i < valid_len; i++){
        result = ((int(*)(const char *, unsigned long))lab11_7)(valid_str[i], strlen(valid_str[i]));
        if (!result) {
	    failLab("Lab 11.7 failed!");
        }
    }
    LAB_VALIDATED(11,7);    
}
