#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static const unsigned char key[] = {
    0xa3, 0x7f, 0x4c, 0xd9, 0x1e, 0xb6, 0x82, 0x5f,
    0x38, 0x0a, 0xed, 0x91, 0x6b, 0xc4, 0x50, 0x2d
};

int main() {
    FILE *f = fopen("/flag", "r");
    if (!f) {
        perror("fopen");
        return 1;
    }

    char hex[1024];
    if (!fgets(hex, sizeof(hex), f)) {
        fclose(f);
        return 1;
    }
    fclose(f);

    size_t hexlen = strlen(hex);
    while (hexlen > 0 && (hex[hexlen - 1] == '\n' || hex[hexlen - 1] == '\r'))
        hex[--hexlen] = '\0';

    size_t len = hexlen / 2;
    unsigned char *enc = malloc(len + 1);

    for (size_t i = 0; i < len; i++)
        sscanf(hex + 2 * i, "%2hhx", &enc[i]);

    int klen = sizeof(key);
    unsigned char *plain = malloc(len + 1);

    for (size_t i = 0; i < len; i++) {
        unsigned char x = enc[i] ^ key[i % klen];
        x = (x >> 3) | (x << 5);
        x ^= 0xaa;
        if (i == 0)
            plain[i] = x ^ key[(len - 1 - i) % klen];
        else
            plain[i] = x ^ key[(len - 1 - i) % klen] ^ enc[i - 1];
    }

    plain[len] = '\0';
    printf("%s\n", plain);

    free(enc);
    free(plain);
    return 0;
}
