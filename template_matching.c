#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h> // Para strcspn

typedef unsigned char uchar;

// Definições para implementar a biblioteca STB (usada APENAS para conversão)
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// =================================================================
// FUNÇÕES DE LEITURA/ESCRITA PGM 
// =================================================================

// Função para ler imagem PGM (formato P2 – texto)
uchar* read_pgm(const char* filename, int* w, int* h) {
    FILE* f = fopen(filename, "r");
    if (!f) {
        perror("Erro abrindo imagem PGM");
        return NULL;
    }
    char tipo[3];
    fscanf(f, "%2s", tipo);
    if (tipo[0] != 'P' || tipo[1] != '2') {
        printf("Formato PGM invalido. Esperado P2.\n");
        fclose(f);
        return NULL;
    }
    int width, height, maxval;
    fscanf(f, "%d %d %d", &width, &height, &maxval);
    uchar* img = malloc(width * height);
    if (!img) {
        printf("Falha ao alocar memoria para PGM\n");
        fclose(f);
        return NULL;
    }
    for (int i = 0; i < width * height; i++) {
        int val;
        if (fscanf(f, "%d", &val) != 1) {
             printf("Erro lendo dados de pixel do PGM\n");
             free(img);
             fclose(f);
             return NULL;
        }
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
    if (!f) {
        perror("Erro ao escrever PGM");
        return;
    }
    fprintf(f, "P2\n%d %d\n255\n", w, h);
    for (int i = 0; i < w * h; i++) {
        fprintf(f, "%d ", img[i]);
        if ((i + 1) % w == 0) fprintf(f, "\n");
    }
    fclose(f);
}

// =================================================================
// FUNÇÃO NCC 
// =================================================================

double ncc(uchar* img, int iw, int ih,
           uchar* tpl, int tw, int th,
           int x0, int y0) {
    
    int N = tw * th;
    if (N == 0) return -1.0;

    double sumI = 0;
    double sumT = 0;

    // --- Primeiro Passe: Calcular as somas ---
    for (int y = 0; y < th; y++) {
        for (int x = 0; x < tw; x++) {
            sumI += img[(y0 + y) * iw + (x0 + x)];
            sumT += tpl[y * tw + x];
        }
    }
    // Calcular as médias
    double avgI = sumI / N;
    double avgT = sumT / N;

    // --- Segundo Passe: Calcular numerador e denominador ---
    double num = 0;
    double den_I = 0; // Variação da Imagem
    double den_T = 0; // Variação do Template

    for (int y = 0; y < th; y++) {
        for (int x = 0; x < tw; x++) {
            double I = img[(y0 + y) * iw + (x0 + x)];
            double T = tpl[y * tw + x];

            double diffI = I - avgI;
            double diffT = T - avgT;

            num += (diffI * diffT);
            den_I += (diffI * diffI);
            den_T += (diffT * diffT);
        }
    }

    // Calcular denominador
    double den = sqrt(den_I * den_T);

    const double epsilon = 1e-6; // Valor muito pequeno
    
    if (den < epsilon) { 
        // Denominador é (quase) zero. Acontece se Imagem ou Template for cor sólida.
        
        if (den_I < epsilon && den_T < epsilon) {
            // AMBOS são de cor sólida (caso 0 / 0)
            // Precisamos checar se as cores são iguais.
            if (fabs(avgI - avgT) < epsilon) {
                return 1.0; // Sólidos E da mesma cor (ex: preto no preto)
            } else {
                return 0.0; // Sólidos de cores diferentes (ex: preto no branco)
            }
        }
        
        // Se só um é sólido, não é uma correspondência.
        return 0.0;
    }

    return num / den;
}

// =================================================================
// FUNÇÃO DE CONVERSÃO 
// =================================================================

/**
 * Converte qualquer formato de imagem (JPG, PNG) para PGM (P2)
 * usando stbi_load() e a função write_pgm().
 * Retorna 1 em sucesso, 0 em falha.
 */
int converter_para_pgm(const char* arquivo_entrada, const char* arquivo_saida_pgm) {
    int w, h, channels;
    // Força o carregamento como 1 canal (tons de cinza)
    uchar* img_data = stbi_load(arquivo_entrada, &w, &h, &channels, 1);

    if (img_data == NULL) {
        fprintf(stderr, "Erro ao carregar imagem %s: %s\n", arquivo_entrada, stbi_failure_reason());
        return 0; // Falha
    }

    // Função para escrever o arquivo PGM (P2)
    write_pgm(arquivo_saida_pgm, img_data, w, h);
    stbi_image_free(img_data);
    
    printf("Convertido: '%s' -> '%s' (PGM P2)\n", arquivo_entrada, arquivo_saida_pgm);
    return 1; // Sucesso
}

// =================================================================
// FUNÇÃO MAIN 
// =================================================================

int main() {
    char img_input[256];
    char tpl_input[256];
    char out_file_pgm[256];

    // Nomes dos arquivos PGM temporários
    const char* temp_img_pgm = "_temp_img.pgm";
    const char* temp_tpl_pgm = "_temp_tpl.pgm";

    // --- Obter nomes dos arquivos do usuário ---
    printf("Digite o nome da imagem principal (qualquer formato): ");
    fgets(img_input, sizeof(img_input), stdin);
    img_input[strcspn(img_input, "\n")] = 0;

    printf("Digite o nome do template (qualquer formato): ");
    fgets(tpl_input, sizeof(tpl_input), stdin);
    tpl_input[strcspn(tpl_input, "\n")] = 0;

    printf("Digite o nome do arquivo de saida (ex: resultado.pgm): ");
    fgets(out_file_pgm, sizeof(out_file_pgm), stdin);
    out_file_pgm[strcspn(out_file_pgm, "\n")] = 0;

    // --- Pré-processamento: Converter para PGM ---
    printf("Convertendo imagens para PGM (P2)...\n");
    if (!converter_para_pgm(img_input, temp_img_pgm)) {
        fprintf(stderr, "Falha ao converter a imagem principal.\n");
        return 1;
    }
    if (!converter_para_pgm(tpl_input, temp_tpl_pgm)) {
        fprintf(stderr, "Falha ao converter o template.\n");
        remove(temp_img_pgm);
        return 1;
    }

    printf("Iniciando logica de template matching (PGM)...\n");
    int iw, ih, tw, th;
    
    uchar* img = read_pgm(temp_img_pgm, &iw, &ih);
    uchar* tpl = read_pgm(temp_tpl_pgm, &tw, &th);

    if (!img || !tpl) {
        if(img) free(img);
        if(tpl) free(tpl);
        remove(temp_img_pgm);
        remove(temp_tpl_pgm);
        return 1;
    }
    
    if (tw > iw || th > ih) {
        fprintf(stderr, "Erro: O template (%d x %d) eh maior que a imagem principal (%d x %d).\n",
                tw, th, iw, ih);
        free(img);
        free(tpl);
        remove(temp_img_pgm);
        remove(temp_tpl_pgm);
        return 1;
    }

    int rw = iw - tw + 1;
    int rh = ih - th + 1;
    uchar* mapa = malloc(iw * ih * sizeof(uchar));
    for (int i = 0; i < iw * ih; i++) mapa[i] = 0;

    double best = -1.0; // Inicia com o pior valor
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

    printf("Melhor correspondencia em x=%d y=%d (NCC=%.3f)\n", bestx, besty, best);

    // Desenha o retângulo branco sobre a imagem original
    for (int x = bestx; x < bestx + tw; x++) {
        if(x < 0 || x >= iw) continue; 
        if(besty >= 0 && besty < ih) img[besty * iw + x] = 255;
        if((besty + th - 1) >= 0 && (besty + th - 1) < ih) img[(besty + th - 1) * iw + x] = 255;
    }
    for (int y = besty; y < besty + th; y++) {
        if(y < 0 || y >= ih) continue;
        if(bestx >= 0 && bestx < iw) img[y * iw + bestx] = 255;
        if((bestx + tw - 1) >= 0 && (bestx + tw - 1) < iw) img[y * iw + (bestx + tw - 1)] = 255;
    }

    // Salva a imagem original (com retângulo) no arquivo PGM final
    write_pgm(out_file_pgm, img, iw, ih);
    printf("Resultado salvo em: %s\n", out_file_pgm);
    
    // ---Limpeza---
    free(img);
    free(tpl);
    free(mapa);
    
    remove(temp_img_pgm);
    remove(temp_tpl_pgm);
    printf("Arquivos temporarios removidos.\n");

    return 0;
}

