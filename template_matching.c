/*
Grupo:

Antonio Carlos Sciamarelli Neto - 10409160
Gustavo Matta - 10410154
Joaquim Rafael Mariano Prieto Pereira - 10408805
Lucas Trebacchetti Eiras - 10401973

Projeto de Template Matching usando Normalized Cross-Correlation (NCC)
Computacao Visual - 2025/2 - 07N - Projeto 2
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h> // Para strcspn e strrchr

typedef unsigned char uchar;

// --- DEFINIÇÕES DAS BIBLIOTECAS STB ---
// STB_IMAGE para carregar (conversor)
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// STB_IMAGE_WRITE para salvar (nova adição)
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"


// =================================================================
// FUNÇÕES PGM 
// =================================================================

uchar* read_pgm(const char* filename, int* w, int* h) {
    FILE* f = fopen(filename, "r");
    if (!f) { perror("Erro abrindo imagem PGM"); return NULL; }
    char tipo[3];
    fscanf(f, "%2s", tipo);
    if (tipo[0] != 'P' || tipo[1] != '2') {
        printf("Formato PGM invalido. Esperado P2.\n"); fclose(f); return NULL;
    }
    int width, height, maxval;
    fscanf(f, "%d %d %d", &width, &height, &maxval);
    uchar* img = malloc(width * height);
    if (!img) { printf("Falha ao alocar memoria para PGM\n"); fclose(f); return NULL; }
    for (int i = 0; i < width * height; i++) {
        int val;
        if (fscanf(f, "%d", &val) != 1) {
             printf("Erro lendo dados de pixel do PGM\n"); free(img); fclose(f); return NULL;
        }
        img[i] = (uchar)val;
    }
    fclose(f); *w = width; *h = height;
    return img;
}

void write_pgm(const char* filename, uchar* img, int w, int h) {
    FILE* f = fopen(filename, "w");
    if (!f) { perror("Erro ao escrever PGM"); return; }
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
double ncc(uchar* img, int iw, int ih, uchar* tpl, int tw, int th, int x0, int y0) {
    int N = tw * th; if (N == 0) return -1.0;
    double sumI = 0, sumT = 0;
    for (int y = 0; y < th; y++) {
        for (int x = 0; x < tw; x++) {
            sumI += img[(y0 + y) * iw + (x0 + x)];
            sumT += tpl[y * tw + x];
        }
    }
    double avgI = sumI / N, avgT = sumT / N;
    double num = 0, den_I = 0, den_T = 0;
    for (int y = 0; y < th; y++) {
        for (int x = 0; x < tw; x++) {
            double I = img[(y0 + y) * iw + (x0 + x)];
            double T = tpl[y * tw + x];
            double diffI = I - avgI, diffT = T - avgT;
            num += (diffI * diffT);
            den_I += (diffI * diffI);
            den_T += (diffT * diffT);
        }
    }
    double den = sqrt(den_I * den_T);
    const double epsilon = 1e-6;
    if (den < epsilon) { 
        if (den_I < epsilon && den_T < epsilon) {
            if (fabs(avgI - avgT) < epsilon) return 1.0; 
            else return 0.0;
        }
        return 0.0;
    }
    return num / den;
}

// =================================================================
// FUNÇÃO DE CONVERSÃO 
// =================================================================
int converter_para_pgm(const char* arquivo_entrada, const char* arquivo_saida_pgm) {
    int w, h, channels;
    uchar* img_data = stbi_load(arquivo_entrada, &w, &h, &channels, 1);
    if (img_data == NULL) {
        fprintf(stderr, "Erro ao carregar imagem %s: %s\n", arquivo_entrada, stbi_failure_reason());
        return 0;
    }
    write_pgm(arquivo_saida_pgm, img_data, w, h);
    stbi_image_free(img_data);
    printf("Convertido: '%s' -> '%s' (PGM P2)\n", arquivo_entrada, arquivo_saida_pgm);
    return 1;
}

// =================================================================
// [NOVA FUNÇÃO] SALVA A IMAGEM FINAL EM QUALQUER FORMATO
// =================================================================
/**
 * Salva o buffer de imagem (tons de cinza, 1 canal) no formato
 * detectado pela extensão do arquivo (PNG, JPG, BMP, ou PGM).
 */
void save_image_output(const char* filename, uchar* img_data, int w, int h) {
    // Encontra a extensão (o último '.' no nome)
    const char *ext = strrchr(filename, '.');

    if (!ext) {
        fprintf(stderr, "Erro: Arquivo de saida '%s' nao tem extensao.\n", filename);
        fprintf(stderr, "Usando '.png' por padrao.\n");
        // Tenta salvar como png se nenhuma extensão for dada
        char new_filename[300];
        snprintf(new_filename, sizeof(new_filename), "%s.png", filename);
        ext = ".png";
        filename = new_filename;
    }

    int success = 0;
    // O buffer 'img_data' está em tons de cinza (1 canal)
    // O 'stride_in_bytes' (passo) é 'w * sizeof(uchar)', ou seja, 'w'.
    
    if (strcmp(ext, ".png") == 0) {
        success = stbi_write_png(filename, w, h, 1, img_data, w * sizeof(uchar));
    } else if (strcmp(ext, ".jpg") == 0 || strcmp(ext, ".jpeg") == 0) {
        success = stbi_write_jpg(filename, w, h, 1, img_data, 90); // 90 = qualidade
    } else if (strcmp(ext, ".bmp") == 0) {
        success = stbi_write_bmp(filename, w, h, 1, img_data);
    } else if (strcmp(ext, ".pgm") == 0) {
        // Mantém a compatibilidade com a sua função original
        write_pgm(filename, img_data, w, h);
        success = 1; // write_pgm não retorna status, assumimos sucesso
    } else {
        fprintf(stderr, "Formato de saida nao suportado: '%s'\n", ext);
        fprintf(stderr, "Use .png, .jpg, .bmp, ou .pgm.\n");
        return;
    }

    if (success) {
        printf("Resultado salvo em: %s\n", filename);
    } else {
        fprintf(stderr, "Erro ao salvar a imagem final em: %s\n", filename);
    }
}


// =================================================================
// FUNÇÃO MAIN 
// =================================================================
int main() {
    char img_input[256];
    char tpl_input[256];
    char out_file_final[256]; 

    const char* temp_img_pgm = "_temp_img.pgm";
    const char* temp_tpl_pgm = "_temp_tpl.pgm";

    // --- Obter nomes dos arquivos do usuário ---
    printf("Digite o nome da imagem principal (qualquer formato): ");
    fgets(img_input, sizeof(img_input), stdin);
    img_input[strcspn(img_input, "\n")] = 0;

    printf("Digite o nome do template (qualquer formato): ");
    fgets(tpl_input, sizeof(tpl_input), stdin);
    tpl_input[strcspn(tpl_input, "\n")] = 0;

    // Pergunta o nome final, pode ser PNG, JPG, etc.
    printf("Digite o nome do arquivo de saida (ex: resultado.png): ");
    fgets(out_file_final, sizeof(out_file_final), stdin);
    out_file_final[strcspn(out_file_final, "\n")] = 0;

    // --- Pré-processamento: Converter para PGM ---
    printf("Convertendo imagens para PGM (P2)...\n");
    if (!converter_para_pgm(img_input, temp_img_pgm)) {
        fprintf(stderr, "Falha ao converter a imagem principal.\n"); return 1;
    }
    if (!converter_para_pgm(tpl_input, temp_tpl_pgm)) {
        fprintf(stderr, "Falha ao converter o template.\n"); remove(temp_img_pgm); return 1;
    }

    // --- Executar a lógica do PGM ---
    printf("Iniciando logica de template matching (PGM)...\n");
    int iw, ih, tw, th;
    uchar* img = read_pgm(temp_img_pgm, &iw, &ih);
    uchar* tpl = read_pgm(temp_tpl_pgm, &tw, &th);

    if (!img || !tpl) {
        if(img) free(img); if(tpl) free(tpl);
        remove(temp_img_pgm); remove(temp_tpl_pgm);
        return 1;
    }
    
    if (tw > iw || th > ih) {
        fprintf(stderr, "Erro: O template (%d x %d) eh maior que a imagem principal (%d x %d).\n", tw, th, iw, ih);
        free(img); free(tpl);
        remove(temp_img_pgm); remove(temp_tpl_pgm);
        return 1;
    }

    int rw = iw - tw + 1, rh = ih - th + 1;
    uchar* mapa = malloc(iw * ih * sizeof(uchar));
    for (int i = 0; i < iw * ih; i++) mapa[i] = 0;

    double best = -1.0; int bestx = 0, besty = 0;

    for (int y = 0; y < rh; y++) {
        for (int x = 0; x < rw; x++) {
            double v = ncc(img, iw, ih, tpl, tw, th, x, y);
            if (v > best) { best = v; bestx = x; besty = y; }
        }
    }
    printf("Melhor correspondencia em x=%d y=%d (NCC=%.3f)\n", bestx, besty, best);

    // Desenha o retângulo na imagem PGM carregada 'img'
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

    // Salva o buffer 'img' função save_image_output
    save_image_output(out_file_final, img, iw, ih);
    
    // --- Limpeza ---
    free(img);
    free(tpl);
    free(mapa);
    remove(temp_img_pgm);
    remove(temp_tpl_pgm);
    printf("Arquivos temporarios removidos.\n");

    return 0;
}


