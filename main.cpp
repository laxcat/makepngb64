#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBI_WRITE_NO_STDIO
#include "stb_image_write.h"

// include directly for simplicity
#include "modp_b64.cc"

bool parseWHC(char const * str, int * w, int * h, int * c) {
    if (str == nullptr) return false;

    int strLen = (int)strlen(str);

    if (strLen >= 512) return false;

    char buff[512];
    strcpy(buff, str);
    char * pen = buff;
    char * base = buff;

    // scan for w
    while (*pen && *pen != 'x') ++pen;
    if (!*pen) return false;
    *pen = '\0';
    unsigned long ww = strtoul(base, nullptr, 10);
    if (ww > INT_MAX) return false;
    *w = (int)ww;
    ++pen;
    base = pen;

    // scan for h
    while (*pen && *pen != 'x') ++pen;
    if (!*pen) return false;
    *pen = '\0';
    unsigned long hh = strtoul(base, nullptr, 10);
    if (hh > INT_MAX) return false;
    *h = (int)hh;
    ++pen;
    base = pen;

    // scan for c
    while (*pen) ++pen;
    unsigned long cc = strtoul(base, nullptr, 10);
    if (cc > INT_MAX) return false;
    *c = (int)cc;

    // must have width and height and 1-4 component count
    if (*w == 0 || *h == 0 || *c == 0 || *c > 4) return false;

    return true;
}

bool parseRGBA(char const * str, int c, uint8_t * r, uint8_t * g, uint8_t * b, uint8_t * a) {
    unsigned long abgr = strtoul(str, nullptr, 16);
    if (abgr == ULLONG_MAX) return false;
    if (c > 0) *r = (abgr >> (c-1)*8) & 0xff;
    if (c > 1) *g = (abgr >> (c-2)*8) & 0xff;
    if (c > 2) *b = (abgr >> (c-3)*8) & 0xff;
    if (c > 3) *a = (abgr >> (c-4)*8) & 0xff;
    return true;
}

int main(int argc, char ** argv) {
    char const * helpMsg = "Usage:\n"
    "./makepngb64 WxHxC 0xRRGGBBAA\n";

    int w, h, c;
    uint8_t r, g, b, a;

    // incorrect usage
    if (argc < 3 ||
        !parseWHC(argv[1], &w, &h, &c) ||
        !parseRGBA(argv[2], c, &r, &g, &b, &a)
    ) {
        printf("%s", helpMsg);
        return 1;
    }

    printf("%dx%d PNG, filled with r:%02x", w, h, r);
    if (c > 1) printf(" g:%02x", g);
    if (c > 2) printf(" b:%02x", b);
    if (c > 3) printf(" a:%02x", a);
    printf(" for every %u-byte pixel:\n", c);

    uint8_t * temp = (uint8_t *)malloc(w*h*c);
    for (int i = 0; i < w*h*c; i += c) {
        temp[i+0] = r;
        if (c > 1) temp[i+1] = g;
        if (c > 2) temp[i+2] = b;
        if (c > 3) temp[i+3] = a;
    }

    int outLen;
    unsigned char * png = stbi_write_png_to_mem(temp, w*c, w, h, c, &outLen);
    char * pngBase64 = (char *)malloc(modp_b64_encode_len(outLen));
    modp_b64_encode(pngBase64, (char const *)png, outLen);
    printf("%s\n", pngBase64);

    free(pngBase64);
    free(png);
    free(temp);

    return 0;
}
