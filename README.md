# Template Matching (Casamento de Modelo) em Visão Computacional

O Template Matching (ou Casamento de Modelo) é uma técnica fundamental em Processamento de Imagens e Visão Computacional. Seu objetivo é **localizar a ocorrência de uma pequena imagem, chamada *template* (ou modelo), dentro de uma imagem maior, que é a imagem de entrada ou origem.**

## Grupo:
* Antonio Carlos Sciamarelli Neto - 10409160
* Gustavo Matta - 10410154
* Joaquim Rafael Mariano Prieto Pereira - 10408805
* Lucas Trebacchetti Eiras - 10401973

## Vídeo Aula:
https://youtu.be/ZYf6ufoEIh4

---

## Como Funciona

O processo de Template Matching utiliza a abordagem de **janela deslizante** (*sliding window*):

1.  **Template:** Define-se a pequena imagem de referência (o modelo) que se deseja encontrar.
2.  **Deslizamento:** O algoritmo "desliza" o template por toda a imagem de entrada, posição por posição.
3.  **Comparação:** Em cada posição, uma **métrica de similaridade** é calculada para medir o quão parecida é a região da imagem coberta pelo template com o próprio template.
4.  **Mapa de Resultados:** Os resultados dessas comparações são armazenados em uma matriz, conhecida como **Mapa de Correlação**.
5.  **Localização:** O ponto nessa matriz que apresenta o valor **máximo** (em métodos de correlação) ou **mínimo** (em métodos de diferença) indica a localização da melhor correspondência na imagem original.

## Métricas de Similaridade Comuns

O cálculo da similaridade é um passo crucial. As métricas mais utilizadas se dividem em duas categorias principais:

| Método | Tipo | Conceito Chave | O que indica o Melhor Match |
| :--- | :--- | :--- | :--- |
| **Correlação Cruzada Normalizada (NCC)** | Similaridade | Mede o quão bem as intensidades de pixels correspondem. É robusta a brilho. | **Valor Mais Alto** ($\approx 1$) |
| **Soma das Diferenças ao Quadrado (SSD)** | Dissimilaridade | Calcula a soma das diferenças quadráticas dos pixels. | **Valor Mais Baixo** ($\approx 0$) |

## Vantagens e Limitações

| Vantagens | Limitações Principais |
| :--- | :--- |
| Simplicidade de implementação. | **Não Invariante** à Escala (Tamanho). |
| Rápido para templates pequenos e bem definidos. | **Não Invariante** à Rotação (Ângulo). |
| Oferece localização precisa (em nível de pixel). | Sensível a grandes mudanças de iluminação e oclusão. |

Para superar as limitações de escala e rotação, utilizam-se variações mais robustas da técnica, como o Template Matching **Multi-Escala** ou abordagens **Baseadas em Características** (Feature-Based).

---

### Aplicações

* Controle de qualidade e inspeção industrial.
* Rastreamento de objetos simples em tempo real.
* Automação de interfaces (*screen matching*).

### Compilação do Código

* Compilar: gcc template_matching.c -o match -lm
* Executar: ./match

### Apresentação

* Link da apresentação referente a template matching: https://prezi.com/view/rfTVyZB4yTKFcD2ugYbS/?referral_token=yiB7C2lnB3FN
