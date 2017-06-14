#include <libc.h>

u32 strlen(const char *str)
{
    u32 i = 0;
    while(str[i] != '\0') {
        i++;
    }
    return i;
}

s32 strcmp(char *s1, char *s2)
{
    u32 i;
    s32 delta;

    if (s1 == NULL && s2 == NULL) {
        return 0;
    }

    if (s1 == NULL && s2 != NULL) {
        return 1;
    }

    if (s1 != NULL && s2 == NULL) {
        return -1;
    }

    /* s1 != NULL && s2 != NULL */
    for(i=0; s1[i]!= '\0' && s2[i] != '\0'; i++) {
        delta = s1[i] - s2[i];
        if (delta != 0) {
            return delta;
        }
    }

    return s1[i] - s2[i];
}

u32 atoi(char *str)
{
    u32 i;
    u32 len;
    u32 sum = 0;

    len = strlen(str);
    if (len == 0) {
        return 0;
    }

    if (len >= 2 && str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) {    /* hex */
        i = 2;
        while(i < len) {
            switch(str[i]) {

                case ('a'): case ('b'): case ('c'): case ('d'): case ('e'): case ('f'):
                    sum = sum*16 + (str[i] - 'a' + 10);
                    break;

                case ('A'): case ('B'): case ('C'): case ('D'): case ('E'): case ('F'):
                    sum = sum*16 + (str[i] - 'A' + 10);
                    break;

                case ('0'): case ('1'): case ('2'): case ('3'): case ('4'):
                case ('5'): case ('6'): case ('7'): case ('8'): case ('9'):
                    sum = sum*16 + (str[i] - '0');
                    break;

                default:
                    return 0;

            }
            i++;
        }
    } else {    /* dec */
        i = 0;
        while(i < len) {
            switch(str[i]) {

                case ('0'): case ('1'): case ('2'): case ('3'): case ('4'):
                case ('5'): case ('6'): case ('7'): case ('8'): case ('9'):
                    sum = sum*10 + (str[i] - '0');
                    break;

                default:
                    return 0;

            }
            i++;
        }

    }
    return sum;
}

void *memset(void *s, s32 c, u32 size)
{
    u32 i;
    char *_s = (char*)s;
    for(i=0;i<size;i++) {
        _s[i] = c;
    }
    return s;
}

s32 memcmp(void *s1, void *s2, u32 n)
{
    u8 *c1, *c2;
    u32 i;
    s32 rv = 0;

    c1 = s1;
    c2 = s2;

    for(i = 0; i < n; i++) {
        if ((rv = (c1[i] - c2[i])) != 0) {
            break;
        }
    }

    return rv;
}

void *memcpy(void *dst, void *src, u32 size)
{
    u32 i;
    u8 *_dst, *_src;

    _dst = (u8 *)dst;
    _src = (u8 *)src;
    for(i=0;i<size;i++) {
        _dst[i] = _src[i];
    }
    return dst;
}

/* find first 1, ffs(0) = 0, ffs(1) = 1, ffs(0x80000010) = 5 */
u32 ffs(u32 x)
{
    u32 i;
    for(i = 0; i < 32; i++) {
        if (x & (1 << i)) {
            return i + 1;
        }
    }

    return 0;
}

/* find last 1, fls(0) = 0, fls(1) = 1, fls(0x80000010) = 32 */
u32 fls(u32 x)
{
    s32 i;
    for(i = 31; i >= 0; i--) {
        if (x & (1 << i)) {
            return i + 1;
        }
    }

    return 0;

}

