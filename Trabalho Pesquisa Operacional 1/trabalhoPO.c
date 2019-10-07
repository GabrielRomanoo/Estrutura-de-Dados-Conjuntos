/* trabalhoPO.c
 * Trabalho de Pesquisa Operacional I - Ciência da Computação
 *
 * Trabalho onde o objetivo é permitir que o usuário entre com informações sobre um problema, que pode ser resolvido pelo método simplex,
 * e, com os dados recebido, resolver o problema pelo método simplex, mostrando cada passo feito.
 *
 * Alunos: Daniel Bou, Gabriel Romano, Jaime Mathias.
 * Data: 05/06/19
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include <float.h>
#include <math.h>

void anal_recursos(int qtde_variaveis, int contador_da_base, double quadro[contador_da_base][qtde_variaveis + contador_da_base], double quadro_inicial[contador_da_base][qtde_variaveis + contador_da_base]);
void anal_objetivo(int qtde_variaveis, int contador_da_base, double quadro[contador_da_base][qtde_variaveis + contador_da_base], double quadro_inicial[contador_da_base][qtde_variaveis + contador_da_base], int base[contador_da_base]);
void minimizacao(int qtde_variaveis, int contador_da_base, double quadro[contador_da_base][qtde_variaveis + contador_da_base], int base[contador_da_base], int * iteracoes);
void maximizacao(int qtde_variaveis, int contador_da_base, double quadro[contador_da_base][qtde_variaveis + contador_da_base], int base[contador_da_base], int * iteracoes);
void imprime_dados(FILE * fp, int qtde_variaveis, int qtde_restricoes, char tipo[4], double funcao_objetivo[qtde_variaveis], double restricoes[qtde_restricoes][qtde_variaveis + 1], char tipo_restricoes[qtde_restricoes][3]);
void monta_quadro(int qtde_variaveis, int qtde_restricoes, int contador_da_base, int k, double funcao_objetivo[qtde_variaveis], double funcao_generalizada[k][qtde_variaveis + 1], double restricoes[qtde_restricoes][qtde_variaveis + 1], double quadro[contador_da_base][qtde_variaveis + contador_da_base]);
void imprime_quadro(int contador_da_base, int qtde_variaveis, double quadro[contador_da_base][qtde_variaveis + contador_da_base], int base[contador_da_base], bool);

int main() {

    FILE * fp;

    char nomearq[50], op[4], k = 0, x;

    int ch, i = 0, j = 0, contador = 0, qtde_variaveis, qtde_restricoes;
    int iteracoes;

    printf("Entre com o nome do arquivo: ");
    scanf(" %49[^\n]", nomearq);

    if ((fp = fopen(nomearq, "r")) == NULL) {
        fprintf(stderr, "Erro de abertura do arquivo %s\n", nomearq);
        printf("Deseja criar um arquivo? (Responda S/N): ");
        scanf(" %4s[^\n]", op);
        if (op[0] == 'N' || op[0] == 'n') {
            return 1;
        }
        if (op[0] == 'S' || op[0] == 's') {
            printf("Informe o nome do arquivo (c/ a extensao): ");
            scanf(" %49[^\n]", nomearq);
            if ((fp = fopen(nomearq, "w+")) == NULL) {
                fprintf(stderr,"Erro de abertura do arquivo");
                return 1;
            }

            printf("Quantas variaveis (maximo 20)? ");
            scanf("%d", &qtde_variaveis);
            fprintf(fp, "%d ", qtde_variaveis);

            printf("Quantas restricoes (maximo 20)? ");
            scanf("%d", &qtde_restricoes);
            fprintf(fp, "%d ", qtde_restricoes);

            char tipo[4];

            printf("Tipo de problema (Max/Min): ");
            scanf(" %s", tipo);
            fprintf(fp, "%s ", tipo);

            double funcao_objetivo[qtde_variaveis];
            double restricoes[qtde_restricoes][qtde_variaveis + 1];
            char tipo_restricoes[qtde_restricoes][3], c[3];

            printf("Funcao Objetivo\n");
            for (i = 0; i < qtde_variaveis; i++) {
                printf("Coeficiente da variavel x%d: ", i);
                scanf("%lf", &funcao_objetivo[i]);
                fprintf(fp, "%lf ", funcao_objetivo[i]);
            }

            for (i = 0; i < qtde_restricoes; i++) {

                printf("Restricao %d\n", i+1);
                for (j = 0; j < qtde_variaveis; j++) {
                    printf("Coeficiente da variavel x%d: ", j);
                    scanf("%lf", &restricoes[i][j]);
                    fprintf(fp, "%lf ", restricoes[i][j]);
                }

                printf("Tipo de Restricao: ");
                scanf(" %s", tipo_restricoes[i]);
                fprintf(fp, "%s ", tipo_restricoes[i]);


                printf("Termo independente: ");
                scanf("%lf", &restricoes[i][j]);
                fprintf(fp, "%lf ", restricoes[i][j]);
            }
        }
    }
    fseek(fp, 0, SEEK_SET);     // Move o cursor dentro do arquivo para o ínicio dele, caso você tenha criado o arquivo na execução

    contador = 0;
    while (!feof(fp)) {
        fscanf(fp, "%d", &ch);
        if (contador == 0) {
            qtde_variaveis = ch;
        }
        if (contador == 1) {
            qtde_restricoes = ch;
            break;
        }
        contador++;
    }

    double funcao_objetivo[qtde_variaveis];
    double restricoes[qtde_restricoes][qtde_variaveis + 1];
    char tipo_restricoes[qtde_restricoes][3], c[3];
    char tipo[4];

    imprime_dados(fp, qtde_variaveis, qtde_restricoes, tipo, funcao_objetivo, restricoes, tipo_restricoes);
    fclose(fp);

    int contador_da_base = 1; // Função objetivo ja conta como uma.

    // Laço para contar quantas equações vamos ter no quadro, porem, quando o sinal da restricao for ==, ela vai se divir em duas equações <= e >=
    for (i = 0; i < qtde_restricoes; i++) {
        if (strcmp(tipo_restricoes[i], "==") == 0) {
            contador_da_base++;
            k++;
        }
        contador_da_base++;
    }

    double funcao_generalizada[k][qtde_variaveis + 1];
    int base[contador_da_base];
    double quadro[contador_da_base][qtde_variaveis + contador_da_base];
    double quadro_inicial[contador_da_base][qtde_variaveis + contador_da_base];

    k = 0;

    for (i = 0; i < qtde_restricoes; i++) {
        for (j = 0; j <= qtde_variaveis; j++) {
            if (tipo_restricoes[i][0] == '>') {
                if (restricoes[i][j] != 0) {    // 0 * -1 em C da -0
                    restricoes[i][j] *= -1;
                }
            }
            else if (tipo_restricoes[i][0] == '=') {
                if (funcao_generalizada[k][j] != 0) {
                    funcao_generalizada[k][j] = restricoes[i][j] * - 1;
                }
            }
        }
        if (tipo_restricoes[i][0] == '=') {
            k++;
        }
    }
    // contador_da_base seria equivalente a quantidade de variaveis de folga, com o acrescimo de z

    for (i = 0; i < contador_da_base; i++) {
        if (i == 0) {
            base[i] = 'z'; // z em ascci
        }
        else {
            base[i] = (qtde_variaveis - 1) + i;
        }
    }

    monta_quadro(qtde_variaveis, qtde_restricoes, contador_da_base, k, funcao_objetivo, funcao_generalizada, restricoes, quadro);

    /* Laço for que grava o quadro inicial. */
    for (i = 0; i < contador_da_base; i++){
        for (j = 0; j < contador_da_base + qtde_variaveis; j++) {
            quadro_inicial[i][j] = quadro[i][j];
        }
    }

    bool generalizado = false, Z_negativo = false;

    /* Laço for que verifica se os sinais das restrições são todos iguais */
    for (i = 1; i < qtde_restricoes; i++) {
        if (strcmp(tipo_restricoes[0], tipo_restricoes[i]) != 0) {
            generalizado = true;
        }
    }

    printf("\nQuadro inicial\n");
    bool viabilizado;
    if (generalizado == true) {
        int contador_negativas;
        iteracoes = 1;
        imprime_quadro(contador_da_base, qtde_variaveis, quadro, base, Z_negativo);
        do {
            viabilizado = false;
            minimizacao(qtde_variaveis, contador_da_base, quadro, base, &iteracoes);
            contador_negativas = qtde_variaveis + contador_da_base - 1;
            /* Laço for para verificar se precisa fazer mais uma iteração de maximização no quadro simplex */
            for (i = 0; i < qtde_variaveis + contador_da_base - 1; i++) {
                if (quadro[0][i] >= 0) {
                    contador_negativas--;
                }
            }
            if (contador_negativas != 0){
                maximizacao(qtde_variaveis, contador_da_base, quadro, base, &iteracoes); // Tem que viabilizar denovo caso de merda
            }
            for (i = 0; i < contador_da_base; i++) {
                if (quadro[i][contador_da_base + qtde_variaveis - 1] < 0.0) {
                      viabilizado = true;
                }
            }
        } while (viabilizado);
        printf("\nSolucao: %.2lf", quadro[0][contador_da_base + qtde_variaveis - 1]);
        for (i = 0; i < qtde_variaveis; i++) {
            for (j = 0; j < contador_da_base; j++) {
                if (base[j] == i) {
                    printf("\nx%d: %.2lf", base[j], quadro[j][contador_da_base + qtde_variaveis - 1]);  // Imprime a solução
                }
            }
        }
    }
    else {
        if (tipo_restricoes[0][0] == '<') {
            iteracoes = 1;
            imprime_quadro(contador_da_base, qtde_variaveis, quadro, base, Z_negativo);
            maximizacao(qtde_variaveis, contador_da_base, quadro, base, &iteracoes);
            printf("\nSolucao: %.2lf", quadro[0][contador_da_base + qtde_variaveis - 1]);
            for (i = 0; i < qtde_variaveis; i++) {
                for (j = 0; j < contador_da_base; j++) {
                    if (base[j] == i) {
                        printf("\nx%d: %.2lf", base[j], quadro[j][contador_da_base + qtde_variaveis - 1]);  // Imprime a solução
                    }
                }
            }
        }
        else {
            if (tipo_restricoes[0][0] == '>'){
                iteracoes = 1;
                Z_negativo = true;
                imprime_quadro(contador_da_base, qtde_variaveis, quadro, base, Z_negativo);
                minimizacao(qtde_variaveis, contador_da_base, quadro, base, &iteracoes);
                printf("\nSolucao: %.2lf", quadro[0][contador_da_base + qtde_variaveis - 1]);
                for (i = 0; i < qtde_variaveis; i++) {
                    for (j = 0; j < contador_da_base; j++) {
                        if (base[j] == i) {
                            printf("\nx%d: %.2lf", base[j], quadro[j][contador_da_base + qtde_variaveis - 1]);  // Imprime a solução
                        }
                    }
                }
            }
        }

    }

    printf("\n\n");
    system("PAUSE");

    printf("\nAnalise de Sensibilidade\n");

    printf("\nRecursos\n");

    anal_recursos(qtde_variaveis, contador_da_base, quadro, quadro_inicial);

    printf("\nCoeficientes do Objetivo\n");

    anal_objetivo(qtde_variaveis, contador_da_base, quadro, quadro_inicial, base);

    return 0;
}


// anal_recursos: procedimento que utiliza os parâmetros passado para fazer a Analise de Sensibilidade dos Recursos
// e imprime o resultado
void anal_recursos(int qtde_variaveis, int contador_da_base, double quadro[contador_da_base][qtde_variaveis + contador_da_base], double quadro_inicial[contador_da_base][qtde_variaveis + contador_da_base]) {
    int i, j, k, x = 1, l;
    double recursos[contador_da_base][contador_da_base - 1];
    double valor_quadro[contador_da_base][2];
    bool unico1 = true, unico2 = true;

    for (i = 1; i < contador_da_base; i++) {
        quadro[i][qtde_variaveis + contador_da_base - 1] *= -1;
        if (fabs(quadro[i][qtde_variaveis + contador_da_base - 1]) == 0) {
            quadro[i][qtde_variaveis + contador_da_base - 1] = 0;
        }
        for (j = 0; j < contador_da_base - 1; j++) {
            recursos[i][j] = DBL_MAX;
        }
    }

    for (j = qtde_variaveis; j < qtde_variaveis + contador_da_base - 1; j++) {
        k = 0;
        for (i = 1; i < contador_da_base; i++) {
            if (quadro[i][j] != 0) {
                recursos[x][k] = quadro[i][qtde_variaveis + contador_da_base - 1] / quadro[i][j];
            }
            if (fabs(recursos[x][k]) == 0) recursos[x][k] = 0;
            for (l = 0; l < 2; l++) {
                valor_quadro[i][l] = 0;
            }
            k++;
        }
        x++;
    }

    for (i = 1; i < contador_da_base; i++) {
        for (j = 0; j < contador_da_base - 1; j++) {
            if (recursos[i][j] <= 0) {
                if (unico1) {
                    valor_quadro[i][0] = recursos[i][j];
                    unico1 = false;
                }
                else if (valor_quadro[i][0] < recursos[i][j]){
                    valor_quadro[i][0] = recursos[i][j];
                }
            }
            else if (recursos[i][j] > 0){
                if (unico2) {
                    valor_quadro[i][1] = recursos[i][j];
                    unico2 = false;
                }
                else if (valor_quadro[i][1] > recursos[i][j]) {
                    valor_quadro[i][1] = recursos[i][j];
                }
            }
        }
        unico1 = true;
        unico2 = true;
    }

    for (j = 0; j < 2; j++) {
        x = qtde_variaveis;
        for (i = 1; i < contador_da_base; i++) {
            if (valor_quadro[i][j] != DBL_MAX) {
            //printf("\n%lf\n", valor_quadro[i][j]);
                valor_quadro[i][j] += quadro_inicial[i][qtde_variaveis + contador_da_base - 1]; //+ Solução inicial da restrição
                if (fabs(valor_quadro[i][j]) == 0) valor_quadro[i][j] = 0;
                if (j == 0) {
                    printf("x%d >= %.2lf\t", x, valor_quadro[i][j]);
                }
                else {
                    printf("x%d <= %.2lf\t", x, valor_quadro[i][j]);
                }

            }
            else {
                if (j == 0) {
                    printf("x%d >= -inf\t", x);
                }
                else {
                    printf("x%d <= +inf\t", x);
                }
            }
            x++;
        }
        printf("\n");
    }
}

// anal_objetivo: procedimento que utiliza os parâmetros passado para fazer a Analise de Sensibilidade da função objetivo
// e imprime o resultado
void anal_objetivo(int qtde_variaveis, int contador_da_base, double quadro[contador_da_base][qtde_variaveis + contador_da_base], double quadro_inicial[contador_da_base][qtde_variaveis + contador_da_base], int base[contador_da_base]) {
    int i, j, k = 0, x, l;
    double objetivo[qtde_variaveis][contador_da_base - 1];
    double valor_quadro[qtde_variaveis][2];
    bool unico1 = true, unico2 = true;

    for (i = 0; i < qtde_variaveis + contador_da_base; i++) {
        quadro[0][i] *= -1;
    }

    for (i = 0; i < qtde_variaveis; i++) {
        for (j = 0; j < contador_da_base - 1; j++) {
            objetivo[i][j] = 0;
        }
        for (l = 0; l < 2; l++) {
            valor_quadro[i][l] = 0;
        }
    }


    for (j = 0; j < qtde_variaveis + contador_da_base - 1; j++) {
        x = 0;
        if (fabs(quadro[0][j]) != 0) {
            for (i = 1; i < contador_da_base; i++) {
                if (fabs(quadro[i][j]) != 0 && (base[i] < qtde_variaveis)) {
                    objetivo[x][k] = quadro[0][j] / quadro[i][j];
                    x++;
                }
            }
            k++;
        }
    }

    for (i = 0; i < qtde_variaveis; i++) {
        for (j = 0; j < contador_da_base - 1; j++) {
            if (objetivo[i][j] < 0) {
                if (unico1) {
                    valor_quadro[i][0] = objetivo[i][j];
                    unico1 = false;
                }
                else if (valor_quadro[i][0] < objetivo[i][j]) {
                    valor_quadro[i][0] = objetivo[i][j];
                }
            }
            else if (objetivo[i][j] > 0){
                if (unico2) {
                    valor_quadro[i][1] = objetivo[i][j];
                    unico2 = false;
                }
                else if (valor_quadro[i][1] > objetivo[i][j]) {
                    valor_quadro[i][1] = objetivo[i][j];
                }
            }
        }
        unico1 = true;
        unico2 = true;
    }

    for (i = 1, k = 0; i < contador_da_base; i++) {
        if (base[i] < qtde_variaveis) {
            base[k++] = base[i];
        }
    }

    for (j = 0; j < 2; j++) {
        x = 0;
        for (i = 0; i < qtde_variaveis; i++) {
            if (fabs(valor_quadro[i][j]) != 0) {
                valor_quadro[i][j] += fabs(quadro_inicial[0][base[i]]); //+ Valor inicial da variavel
                if (valor_quadro[i][j] == quadro_inicial[0][i]) {
                    if (j == 0) {
                        printf("c%d >= -inf\t", base[i]);
                    }
                    else {
                        printf("c%d <= +inf\t", base[i]);
                    }
                }
                else {
                    if (j == 0) {
                        printf("c%d >= %.2lf\t", base[i], valor_quadro[i][j]);
                    }
                    else {
                        printf("c%d <= %.2lf\t", base[i], valor_quadro[i][j]);
                    }
                }
            }
            else {
                if (j == 0) {
                    printf("c%d >= -inf\t", base[i]);
                }
                else {
                    printf("c%d <= +inf\t", base[i]);
                }
            }
            x++;
        }
        printf("\n");
    }
}

// minimizacao: procedimento que utiliza os parâmetros para fazer o Método Simplex para prolemas de Minimização
// e imprime as variáveis que saem e entram da base
void minimizacao(int qtde_variaveis, int contador_da_base, double quadro[contador_da_base][qtde_variaveis + contador_da_base], int base[contador_da_base], int * iteracoes) {
    int contador_negativas, i, j;

    do {
        printf("\n");
        system("PAUSE");
        double mais_neg = quadro[1][qtde_variaveis + contador_da_base - 1] * -1;
        int posicao_linha = 1;
        for (i = 2; i < contador_da_base; i++) {
            if (fabs(quadro[i][qtde_variaveis + contador_da_base - 1])!= 0) {
                if (quadro[i][qtde_variaveis + contador_da_base - 1] * -1 >= mais_neg) {
                    mais_neg = quadro[i][qtde_variaveis + contador_da_base - 1] * -1;
                    posicao_linha = i;
                }
            }
        }

        int comparador = INT_MAX, posicao_coluna = 0;
        double divisao;
        for (i = 0; i < qtde_variaveis + contador_da_base; i++) {
            if (fabs(quadro[0][i]) != 0 && fabs(quadro[posicao_linha][i]) != 0) {
                divisao = (quadro[0][i] / quadro[posicao_linha][i]) * 1000;
                if (divisao < 0.0) {
                    divisao = divisao * -1;
                }
                if (divisao <= comparador) {
                    comparador = divisao;
                    posicao_coluna = i;
                }
            }
        }

        double pivo = quadro[posicao_linha][posicao_coluna];
        double nlp[qtde_variaveis + contador_da_base];          // NLP = nova linha do pivo
        for (i = 0; i < qtde_variaveis + contador_da_base; i++) {
            if (fabs(quadro[posicao_linha][i]) != 0) {
                quadro[posicao_linha][i] /= pivo;
            }
            nlp[i] = quadro[posicao_linha][i];
        }

        for (i = 0; i < contador_da_base; i++) {
            if (quadro[i][posicao_coluna] == 0) {
                pivo = quadro[i][posicao_coluna];
            }
            else {
                pivo = quadro[i][posicao_coluna] * -1;
            }
            for (j = 0; j < qtde_variaveis + contador_da_base; j++) {
                if (i != posicao_linha) {
                    if (nlp[j] != 0) {
                        quadro[i][j] = pivo * nlp[j] + quadro[i][j];
                    }
                    else {
                        quadro[i][j] = nlp[j] + quadro[i][j];
                    }
                }
                if (fabs(quadro[i][j]) == 0) {
                    quadro[i][j] = 0;
                }
            }
        }

        printf("\nIteracao %d: sai x%d, entra x%d\n", *iteracoes, base[posicao_linha], posicao_coluna);
        (*iteracoes)++;
        base[posicao_linha] = posicao_coluna;
        imprime_quadro(contador_da_base, qtde_variaveis, quadro, base, true);
        contador_negativas = contador_da_base - 1;
        for (i = 1; i < contador_da_base; i++) {
            if (quadro[i][qtde_variaveis + contador_da_base - 1] >= 0) {    // CONTADOR DA BASE -1 = QTDE_RESTRICOES
                contador_negativas--;
            }
        }
    } while (contador_negativas != 0);
}

// maximizacao: procedimento que utiliza os parâmetros para fazer o Método Simplex para prolemas de Maximização
// e imprime as variáveis que saem e entram da base
void maximizacao(int qtde_variaveis, int contador_da_base, double quadro[contador_da_base][qtde_variaveis + contador_da_base], int base[contador_da_base], int * iteracoes){
    int contador_negativas, i, j;

    do {
        printf("\n");
        system("PAUSE");

        double menor = quadro[0][0];
        int posicao_coluna = 0;
        for (i = 1; i <= contador_da_base; i++) {
            if (quadro[0][i] < menor) {
                menor = quadro[0][i];
                posicao_coluna = i;
            }
        }

        int comparador = INT_MAX, posicao_linha = 1;
        double divisao;
        for (i = 1; i < contador_da_base; i++) {
            divisao = quadro[i][qtde_variaveis + contador_da_base - 1] / quadro[i][posicao_coluna];
            if (fabs(divisao) == 0) {
                divisao = 0;
            }
            if ((divisao <= comparador) && divisao > 0.0) {
                comparador = divisao;
                posicao_linha = i;
            }
        }

        double pivo = quadro[posicao_linha][posicao_coluna];
        double nlp[qtde_variaveis + contador_da_base];   // NLP = nova linha do pivo
        for (i = 0; i < qtde_variaveis + contador_da_base; i++) {
            quadro[posicao_linha][i] = quadro[posicao_linha][i] / pivo;
            nlp[i] = quadro[posicao_linha][i];
        }
        for (i = 0; i < contador_da_base; i++) {
            pivo = quadro[i][posicao_coluna] * -1;
            if (fabs(pivo) == 0) {
                pivo = 0;
            }
            for (j = 0; j < qtde_variaveis + contador_da_base; j++) {
                if (i != posicao_linha) {
                    quadro[i][j] = pivo * nlp[j] + quadro[i][j];
                }
            }
        }

        printf("\nIteracao %d: sai x%d, entra x%d\n", *iteracoes, base[posicao_linha], posicao_coluna);
        (*iteracoes)++;
        base[posicao_linha] = posicao_coluna;
        imprime_quadro(contador_da_base, qtde_variaveis, quadro, base, false);
        contador_negativas = qtde_variaveis + contador_da_base - 1;
        for (i = 0; i < qtde_variaveis + contador_da_base - 1; i++) {
            if (quadro[0][i] >= 0) {
                contador_negativas--;
            }
        }
    } while (contador_negativas != 0);
}

// imprime_dados: procedimento que utiliza os parâmetros para imprimir os dados com que o quadro simplex será feito.
void imprime_dados(FILE * fp, int qtde_variaveis, int qtde_restricoes, char tipo[4], double funcao_objetivo[qtde_variaveis], double restricoes[qtde_restricoes][qtde_variaveis + 1], char tipo_restricoes[qtde_restricoes][3]) {
    fscanf(fp, "%s", tipo);
    printf("Quantidade de variaveis: %d\nQuantidade de restricoes: %d\n\nTipo de Problema: %s\n\n", qtde_variaveis, qtde_restricoes, tipo);

    int i, j, c[3];
    double ch;

    for (i = 0; i < qtde_variaveis; i++) {
        fscanf(fp, "%lf", &ch);
        funcao_objetivo[i] = ch;
    }

    printf("Funcao Objetivo");
    for (i = 0; i < qtde_variaveis; i++) {
        printf("\nCoeficiente da variavel x%d: %.2lf", i, funcao_objetivo[i]);
    }

    if (strcmp(tipo,"Max") == 0){

        for (i = 0; i < qtde_variaveis; i++) {
            funcao_objetivo[i] *= -1;
        }


    }
    printf("\n\n");

    for (i = 0; i < qtde_restricoes; i++) {
        printf("Restricao %d", i+1);
        for (j = 0; j < qtde_variaveis; j++) {
            fscanf(fp, "%lf", &ch);
            restricoes[i][j] = ch;
            printf("\nCoeficiente da variavel x%d: %.2lf", j, restricoes[i][j]);
        }

        fscanf(fp, "%s", tipo_restricoes[i]);
        printf("\nTipo de Restricao: %s", tipo_restricoes[i]);

        fscanf(fp, "%lf", &ch);
        restricoes[i][j] = ch;
        printf("\nTermo independente: %.2lf", restricoes[i][j]);
        printf("\n\n");
    }

}

// monta_quadro: procedimento que utiliza os parâmetros e otimiza os dados para serem inseridos numa só matriz
// deixando do jeito de um quadro simplex
void monta_quadro(int qtde_variaveis, int qtde_restricoes, int contador_da_base, int k, double funcao_objetivo[qtde_variaveis], double funcao_generalizada[k][qtde_variaveis + 1], double restricoes[qtde_restricoes][qtde_variaveis + 1], double quadro[contador_da_base][qtde_variaveis + contador_da_base]) {
    int i, j;

    k = 0;
    for (i = 0; i < contador_da_base; i++) {

        for (j = 0; j < qtde_variaveis + contador_da_base; j++) {
            if (i == 0 && j < qtde_variaveis) {
                quadro[i][j] = funcao_objetivo[j];
            }
            else if (i > qtde_restricoes && j < qtde_variaveis) {
                quadro[i][j] = funcao_generalizada[k][j];
            }
            else if (j < qtde_variaveis){
                quadro[i][j] = restricoes[i - 1][j];
            }
            else if (j == qtde_variaveis + contador_da_base - 1 && i != 0) { // Coluna da Solução
                if (i > qtde_restricoes) {
                    quadro[i][j] = funcao_generalizada[k][qtde_variaveis];
                }
                else {
                    quadro[i][j] = restricoes[i - 1][qtde_variaveis];
                }
            }
            else if (j == qtde_variaveis + i - 1) {     // qtde_variaveis + i para depois de todas as variaveis fazer o 1 da folga, e o -1 é por causa da função objetivo
                quadro[i][j] = 1;
            }
            else {
                quadro[i][j] = 0;
            }
        }
        if (i > qtde_restricoes) {
            k++;
        }
    }
}

// imprime_dados: procedimento que utiliza os parâmetros e imprime eles na forma do quadro simplex
void imprime_quadro(int contador_da_base, int qtde_variaveis, double quadro[contador_da_base][qtde_variaveis + contador_da_base], int base[contador_da_base], bool Z_negativo) {
    int i, j;

    printf(" B\t");
    for (i = 0; i < contador_da_base + qtde_variaveis - 1; i++) {
        printf("x%d\t", i);
    }
    printf("Sol");
    printf("\n");

    for (i = 0; i < contador_da_base; i++) {

        if (i != 0) {
            printf("x%d\t", base[i]);
        }
        else if (Z_negativo == false){
            printf(" %c\t", base[i]);
        }
        else {
            printf("-%c\t", base[i]);
        }

        for (j = 0; j < qtde_variaveis + contador_da_base; j++) {

            printf("%.2lf\t", quadro[i][j]);
        }
        printf("\n");
    }
}
