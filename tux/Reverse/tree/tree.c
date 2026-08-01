#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define N 15

typedef struct Node {
    unsigned char val;
    struct Node *left;
    struct Node *right;
} Node;

static const int LEFT[N] = {1, 3, 5, 7, 9, 11, 13, -1, -1, -1, -1, -1, -1, -1, -1};
static const int RIGHT[N] = {2, 4, 6, 8, 10, 12, 14, -1, -1, -1, -1, -1, -1, -1, -1};

static const unsigned char PRE[15] = {
    0x49, 0x40, 0x43, 0x34, 0x62, 0x7E, 0x33, 0x32,
    0x57, 0x71, 0x60, 0x3E, 0x77, 0x70, 0x7A
};
static const unsigned char IN[15] = {
    0x34, 0x43, 0x62, 0x40, 0x33, 0x7E, 0x32, 0x49,
    0x60, 0x71, 0x3E, 0x57, 0x70, 0x77, 0x7A
};

static Node *new_node(unsigned char c) {
    Node *n = (Node *)calloc(1, sizeof(Node));
    n->val = c;
    return n;
}

static Node *build_tree(const unsigned char *s) {
    Node *nodes[N];
    int i;
    for (i = 0; i < N; i++)
        nodes[i] = new_node(s[i]);
    for (i = 0; i < N; i++) {
        if (LEFT[i] >= 0)
            nodes[i]->left = nodes[LEFT[i]];
        if (RIGHT[i] >= 0)
            nodes[i]->right = nodes[RIGHT[i]];
    }
    return nodes[0];
}

static unsigned char enc(unsigned char c, int depth) {
    return (unsigned char)(c ^ (depth * 2 + 1));
}

static void preorder(Node *n, unsigned char *out, int *k, int depth) {
    if (!n)
        return;
    out[(*k)++] = enc(n->val, depth);
    preorder(n->left, out, k, depth + 1);
    preorder(n->right, out, k, depth + 1);
}

static void inorder(Node *n, unsigned char *out, int *k, int depth) {
    if (!n)
        return;
    inorder(n->left, out, k, depth + 1);
    out[(*k)++] = enc(n->val, depth);
    inorder(n->right, out, k, depth + 1);
}

int main(int argc, char **argv) {
    unsigned char pre[15], in[15];
    int k;

    if (argc != 2) {
        printf("usage: %s <flag>\n", argv[0]);
        return 1;
    }
    if (strlen(argv[1]) != N) {
        puts("wrong");
        return 1;
    }

    k = 0;
    preorder(build_tree((const unsigned char *)argv[1]), pre, &k, 0);
    k = 0;
    inorder(build_tree((const unsigned char *)argv[1]), in, &k, 0);

    if (memcmp(pre, PRE, N) == 0 && memcmp(in, IN, N) == 0) {
        puts("correct!");
        return 0;
    }
    puts("wrong");
    return 1;
}
