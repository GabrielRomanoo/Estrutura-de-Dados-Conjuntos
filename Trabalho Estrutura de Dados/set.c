/* set.c
 * Implementação das operações que podem ser realizadas sobre o
 * TAD set
*/

#include "set.h"
#include <stdio.h>

void set_init(set * s)
{
    s->n = 0;
}

bool set_isempty(set s)
{
    return (s.n == 0);
}

bool set_isfull(set s)
{
    return (s.n == SET_MAX);
}

bool set_member(set s, set_info x)
{
    int i;
    for (i = 0; i < s.n; i++) {
        /* verifica se a variável x está na posição i do conjunto s */
        if (x == s.itens[i]) {
            return true;
        }
    }
    return false;
}

bool set_insert(set * s, set_info x)
{
    /* verifica se o conjunto s não está cheio e se elemento x não é repetido no conjunto s */
    if (!set_isfull(*s) && !set_member(*s, x)) {
        s->itens[s->n++] = x;
        return true;
    }
    return false;
}

bool set_remove(set * s, set_info x)
{
    /* verifica se a variável x está no conjunto s */
    if (set_member(*s, x)) {
        int i, j;
        /* o laço de repetição abaixo procura a posição do elemento à ser removido */
        for (i = 0; i < s->n; i++) {
            if (s->itens[i] == x) {
                /* a variável j guarda a posição do elemento à ser removido */
                j = i;
                break;
            }
        }
        /* o laço de repetição abaixo desloca todos os elementos para as suas novas posições,
           começando pela posição j do elemento à ser removido */
        for (i = j; i < s->n - 1; i++) {
            s->itens[i] = s->itens[i+1];
        }
        s->n--;
        return true;
    }
    return false;
}

bool set_union(set s1, set s2, set * s3)
{
    int i, repetidos = 0;
    /* verifica a quantidade de membros em comum entre os conjuntos s1 e s2 */
    for (i = 0; i < s1.n; i++) {
        if (set_member(s2, s1.itens[i])) {
            repetidos++;
        }
    }
    /* verifica se haverá estouro na capacidade do conjunto s3 */
    if (s1.n + s2.n - repetidos <= SET_MAX) {
        set_init(s3);
        /* o laço de repetição abaixo atribui todos os elementos do conjunto s1 em s3 */
        for (i = 0; i < s1.n; i++) {
            s3->itens[s3->n++] = s1.itens[i];
        }
        /* o laço de repetição abaixo atribui todos os elementos do conjunto s2 em s3 que já não esteja em s3 */
        for (i = 0; i < s2.n; i++) {
                /* como já foram atribuídos todos os elementos de s1 em s3, o if abaixo
                   verifica se existe algum elemento repetido de s2 em s3 */
                if (!set_member(*s3, s2.itens[i])) {
                    s3->itens[s3->n++] = s2.itens[i];
                }
        }
        return true;
    }
    return false;
}

void set_intersection(set s1, set s2, set * s3)
{
    set_init(s3);
    int i;
    for (i = 0; i < s1.n; i++) {
        /* verifica os membros em comum entre s1 e s2 */
        if (set_member(s2, s1.itens[i]) ) {
            s3->itens[s3->n++] = s1.itens[i];
        }
    }
}

void set_difference(set s1, set s2, set * s3)
{
    set_init(s3);
    int i;
    for (i = 0; i < s1.n; i++) {
        /* verifica se o membro na posição i de s1 não se repete em s2 */
        if (!set_member(s2, s1.itens[i])) {
            s3->itens[s3->n++] = s1.itens[i];
        }
    }
}

void set_print(set s)
{
    int i;
	printf("[ ");
	for (i = 0; i < s.n; i++) {
		printf("%d,", s.itens[i]);
	}
	printf("\b ]\n");
}

/*---------------------
        AVALIAÇÃO
  ---------------------
  - Corretude...: 10.00
  - Interface...: 10.00
  - Legibilidade: 10.00
  - Nota........: 10.00
  ---------------------*/
