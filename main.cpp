#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBI_WRITE_NO_STDIO
#include "stb_image_write.h"

// include directly for simplicity
#include "modp_b64.cc"

bool parseInt(char const ** str, int * i) {
    if (!**str) return false;
    *i = 0;
    while (**str >= '0' && **str <= '9') {
        *i = *i * 10 + (**str - '0');
        ++*str;
    }
    if (**str && **str != 'x' && **str != 'X') return false;
    return true;
}

bool parseWHC(char const * str, int * w, int * h, int * c) {
    // scan for w
    if (!parseInt(&str, w) || !*str) return false;
    ++str;

    // scan for h
    if (!parseInt(&str, h) || !*str) return false;
    ++str;

    // scan for c
    if (!parseInt(&str, c) || *str) return false;

    // must have width and height and 1-4 component count
    if (*w == 0 || *h == 0 || *c == 0 || *c > 4) return false;

    return true;
}

bool parseRGBA(char const * str, int c, uint8_t * r, uint8_t * g, uint8_t * b, uint8_t * a) {
    unsigned long color = strtoul(str, nullptr, 16);
    if (color == ULLONG_MAX) return false;
               *r = color >> (c-1)*8 & 0xff;
    if (c > 1) *g = color >> (c-2)*8 & 0xff;
    if (c > 2) *b = color >> (c-3)*8 & 0xff;
    if (c > 3) *a = color >> (c-4)*8 & 0xff;
    return true;
}

int main(int argc, char ** argv) {
    int w, h, c;
    uint8_t r, g, b, a;

    // parse parameters and output usage message on fail
    if (argc < 3 ||
        !parseWHC(argv[1], &w, &h, &c) ||
        !parseRGBA(argv[2], c, &r, &g, &b, &a)
    ) {
        printf("Usage:\nmakepngb64 WxHxC 0xRRGGBBAA\n");
        return 1;
    }

    // output description line
    printf("%dx%d PNG, filled with 0x", w, h);
               printf("%02x", r);
    if (c > 1) printf("%02x", g);
    if (c > 2) printf("%02x", b);
    if (c > 3) printf("%02x", a);
               printf(" (r");
    if (c > 1) printf(",g");
    if (c > 2) printf(",b");
    if (c > 3) printf(",a");
    printf(") for every %u-byte pixel:\n", c);

    // fill unencoded pixel buffer
    int pixelsLen = w*h*c;
    uint8_t * pixels = (uint8_t *)malloc(pixelsLen);
    for (int i = 0; i < pixelsLen; i += c) {
                   pixels[i+0] = r;
        if (c > 1) pixels[i+1] = g;
        if (c > 2) pixels[i+2] = b;
        if (c > 3) pixels[i+3] = a;
    }

    // enocde to png, then base-64 string
    int pngLen;
    unsigned char * png = stbi_write_png_to_mem(pixels, w*c, w, h, c, &pngLen);
    char * pngBase64 = (char *)malloc(modp_b64_encode_len(pngLen));
    modp_b64_encode(pngBase64, (char const *)png, pngLen);
    printf("%s\n", pngBase64);

    // cleanup and return success
    free(pngBase64);
    free(png);
    free(pixels);
    return 0;
}
