# 🔍 Template Matching (Casamento de Modelo) em Visão Computacional

Template Matching é uma técnica fundamental em Processamento de Imagens e Visão Computacional usada para **localizar a ocorrência de uma pequena imagem (o *template* ou modelo) dentro de uma imagem maior (a imagem de entrada/origem).**

---

## 💡 Como Funciona

O processo de Template Matching utiliza a técnica de **janela deslizante** (ou *sliding window*):

1.  **Template:** Uma pequena imagem de referência (o modelo) que se deseja encontrar.
2.  **Deslizamento:** O algoritmo move o *template* para cada posição possível sobre a imagem de entrada.
3.  **Comparação:** Em cada posição, uma **métrica de similaridade** é calculada para medir o quão parecido é o *template* com a região da imagem que ele cobre.
4.  **Mapa de Resultados:** Os resultados das comparações são armazenados em uma matriz (o **Mapa de Correlação**).
5.  **Localização:** O ponto no Mapa de Resultados com o valor **máximo** (para correlação) ou **mínimo** (para diferença) indica a melhor correspondência na imagem original.

## 📐 Métricas de Similaridade Comuns

O cálculo da similaridade é crucial. As métricas mais utilizadas caem em duas categorias:

| Método | Tipo | Conceito Chave | O que indica o Melhor Match |
| :--- | :--- | :--- | :--- |
| **Correlação Cruzada Normalizada (NCC)** | Similaridade | Mede o quão bem as intensidades de pixels correspondem. É robusta a brilho. | **Valor Mais Alto** ($\approx 1$) |
| **Soma das Diferenças ao Quadrado (SSD)** | Dissimilaridade | Calcula a soma das diferenças quadráticas dos pixels. | **Valor Mais Baixo** ($\approx 0$) |

## 🚀 Vantagens e Limitações

| Vantagens | Limitações Principais |
| :--- | :--- |
| ✅ Simplicidade de implementação. | ❌ **Não Invariante** à Escala (Tamanho). |
| ✅ Rápido para templates pequenos e bem definidos. | ❌ **Não Invariante** à Rotação (Ângulo). |
| ✅ Oferece localização precisa (em nível de pixel). | ❌ Sensível a grandes mudanças de iluminação e oclusão. |

Para lidar com escala e rotação, são usadas variações mais robustas, como o Template Matching **Multi-Escala** ou abordagens **Baseadas em Características**.

---

### **Aplicações**

* Controle de qualidade e inspeção industrial.
* Rastreamento de objetos simples em tempo real.
* Automação de interfaces (*screen matching*).

