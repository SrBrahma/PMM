/* Rapidinha feita pelo Henrique Bruno, para nosso camarada Loã. */

#include <stdio.h>      /* printf */
#include <math.h>       /* sqrt */
#include <string.h>     /* fgets, strtof */
#include <stdlib.h>     /* strtof */

#define OK                  0
#define MONITOR_COLUNAS     20
#define MONITOR_LINHAS      10
#define TAMANHO_BUFFER      64
#define EOS                 '\0'


int
CalcularRaizes(float valorA, float valorB, float valorC)
{
    float delta, resultado1, resultado2;

    delta = (valorB * valorB) - 4 *  valorA * valorC;
    if (delta < 0)
    {
        printf("Delta negativo, raiz inexistente.\n\n");
        return 0;
    }
    /* Else implicito. */
    resultado1 = (-valorB + sqrt(delta)) / 2 * valorA;
    resultado2 = (-valorB - sqrt(delta)) / 2 * valorA;
    printf("Primeira raiz = %f, Segunda raiz = %f\n\n", resultado1, resultado2);
    return 1;
}


int
main()
{
    char letras[] = "abc";
    unsigned short letraAtual = 0, numeroLetras = strlen(letras);
    char buffer[TAMANHO_BUFFER] = "";
    float valor[numeroLetras];
    char *ptrValidacao;
    unsigned short sairDoPrograma = 0;

    while (!sairDoPrograma)
    {
        printf("Entre com o valor do valor de %c: ", letras[letraAtual]);
        fgets(buffer, TAMANHO_BUFFER, stdin);
        buffer[strlen(buffer) - 1] = EOS;

        if (!strcmp(buffer, "exit"))
            sairDoPrograma = 1;

        else
        {
            valor[letraAtual] = strtof(buffer, &ptrValidacao);

            if(*ptrValidacao != EOS)
                printf("Entrada invalida (\"%c\"). Entre somente com floats. (Separador = \'.\').\n", *ptrValidacao);

            else
            {
                letraAtual ++;
                if (letraAtual == numeroLetras)
                {
                    CalcularRaizes(valor[0], valor[1], valor[2]);
                    letraAtual = 0;
                }
            }
        }
    }
    return OK;
}
