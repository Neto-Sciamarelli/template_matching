#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef unsigned char uchar;

// Função para ler imagem PGM (formato P2 – texto)
uchar* read_pgm(const char* filename, int* w, int* h) {
    FILE* f = fopen(filename, "r");
    if (!f) {
        perror("Erro abrindo imagem");
        return NULL;
    }

    char tipo[3];
    fscanf(f, "%2s", tipo);
    if (tipo[0] != 'P' || tipo[1] != '2') {
        printf("Formato inválido. Use imagens PGM (P2).\n");
        fclose(f);
        return NULL;
    }

    int width, height, maxval;
    fscanf(f, "%d %d %d", &width, &height, &maxval);

    uchar* img = malloc(width * height);
    for (int i = 0; i < width * height; i++) {
        int val;
        fscanf(f, "%d", &val);
        img[i] = (uchar)val;
    }

    fclose(f);
    *w = width;
    *h = height;
    return img;
}

// Função para escrever imagem PGM (P2)
void write_pgm(const char* filename, uchar* img, int w, int h) {
    FILE* f = fopen(filename, "w");
    fprintf(f, "P2\n%d %d\n255\n", w, h);
    for (int i = 0; i < w * h; i++) {
        fprintf(f, "%d ", img[i]);
        if ((i + 1) % w == 0) fprintf(f, "\n");
    }
    fclose(f);
}

// Cálculo da correlação normalizada (NCC)
double ncc(uchar* img, int iw, int ih,
           uchar* tpl, int tw, int th,
           int x0, int y0) {
    double sumI = 0, sumT = 0, sumI2 = 0, sumT2 = 0, sumIT = 0;
    int N = tw * th;

    for (int y = 0; y < th; y++) {
        for (int x = 0; x < tw; x++) {
            double I = img[(y0 + y) * iw + (x0 + x)];
            double T = tpl[y * tw + x];
            sumI += I;
            sumT += T;
            sumI2 += I * I;
            sumT2 += T * T;
            sumIT += I * T;
        }
    }

    double num = sumIT - (sumI * sumT) / N;
    double den = sqrt((sumI2 - (sumI * sumI) / N) * (sumT2 - (sumT * sumT) / N));
    if (den == 0) return 0;
    return num / den;
}

int main() {
    int iw, ih, tw, th;
    uchar* img = read_pgm("imagem.pgm", &iw, &ih);
    uchar* tpl = read_pgm("template.pgm", &tw, &th);

    if (!img || !tpl) return 1;

    int rw = iw - tw + 1;
    int rh = ih - th + 1;
    uchar* mapa = malloc(iw * ih);
    for (int i = 0; i < iw * ih; i++) mapa[i] = 0;

    double best = -1.0;
    int bestx = 0, besty = 0;

    for (int y = 0; y < rh; y++) {
        for (int x = 0; x < rw; x++) {
            double v = ncc(img, iw, ih, tpl, tw, th, x, y);
            if (v > best) {
                best = v;
                bestx = x;
                besty = y;
            }
        }
    }

    printf("Melhor correspondência em x=%d y=%d (NCC=%.3f)\n", bestx, besty, best);

    // desenha o retângulo do template no mapa de saída
    for (int x = bestx; x < bestx + tw; x++) {
        mapa[besty * iw + x] = 255;
        mapa[(besty + th - 1) * iw + x] = 255;
    }
    for (int y = besty; y < besty + th; y++) {
        mapa[y * iw + bestx] = 255;
        mapa[y * iw + (bestx + tw - 1)] = 255;
    }

    // Desenha o retângulo branco sobre a imagem original
    for (int x = bestx; x < bestx + tw; x++) {
        img[besty * iw + x] = 255;
        img[(besty + th - 1) * iw + x] = 255;
    }
    for (int y = besty; y < besty + th; y++) {
        img[y * iw + bestx] = 255;
        img[y * iw + (bestx + tw - 1)] = 255;
    }

    // Salva a imagem original com o retângulo
    write_pgm("resultado.pgm", img, iw, ih);

    free(img);
    free(tpl);
    free(mapa);
    return 0;
}
