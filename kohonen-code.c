#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <math.h>
#include <time.h>

#define MAPA_LADO_X 10
#define MAPA_LADO_Y 10
#define DIMENSOES 14
#define AMOSTRAS 178
#define ITERACOES_MAX 5000 // at least 500 times the number of network units

#define ALPHA_INICIAL 0.6
#define RAIO_INICIAL ((MAPA_LADO_X > MAPA_LADO_Y ? MAPA_LADO_X : MAPA_LADO_Y) / 2.0)
#define TEMPO_CONSTANTE (ITERACOES_MAX / 4.0) // Constante de tempo para decaimento

int main()
{
    // Verificar se a classe deve ser separada do mapa de kohonen mesmo
    int x, y, d;
    float mapa[MAPA_LADO_X][MAPA_LADO_Y][DIMENSOES];

    float dados_entrada[AMOSTRAS][DIMENSOES];

    // LEITURA DOS DADOS
    FILE *in;

    if ((in = fopen("wine.data", "rt")) == NULL)
    {
        printf("Cannot open input file.\n");
        return 1;
    }

    x = 0;
    while (!feof(in) && x < AMOSTRAS)
    {
        int result = fscanf(in, "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f",
                            &dados_entrada[x][0], &dados_entrada[x][1], &dados_entrada[x][2],
                            &dados_entrada[x][3], &dados_entrada[x][4], &dados_entrada[x][5],
                            &dados_entrada[x][6], &dados_entrada[x][7], &dados_entrada[x][8],
                            &dados_entrada[x][9], &dados_entrada[x][10], &dados_entrada[x][11],
                            &dados_entrada[x][12], &dados_entrada[x][13]);

        if (result == 14)
        {
            x++;
        }
        else
        {
            break;
        }
    }

    // printf("Total de amostras lidas: %d\n", i);
    // for (j = 0; j < i; j++)
    // {
    //     printf("Amostra %d - Classe: %d | Atributos: %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f\n",
    //            j, E1[j], E2[j], E3[j], E4[j], E5[j], E6[j], E7[j], E8[j], E9[j], E10[j], E11[j], E12[j], E13[j]);
    // }

    // srand(time(NULL)); verificar se é necessario usar essa seed

    for (x = 0; x < MAPA_LADO_X; x++) // linha
    {
        for (y = 0; y < MAPA_LADO_Y; y++) // coluna
        {
            for (d = 0; d < DIMENSOES; d++) // atributo/peso
            {
                mapa[x][y][d] = (float)rand() / RAND_MAX;
            }
        }
    }
    // Mostrar uma representação visual do mapa
    printf("\n=== MAPA INICIAL (Pesos Aleatorios) ===\n");

    for (x = 0; x < MAPA_LADO_X; x++)
    {
        for (y = 0; y < MAPA_LADO_Y; y++)
        {
            // Mostrar o primeiro peso (índice 0) de cada neurônio
            printf("%.2f ", mapa[x][y][1]);
        }
        printf("\n");
    }

    int v_x, v_y; // Coordenadas do vencedor (v)
    for (int t = 1; t <= ITERACOES_MAX; t++)
    {
        int indice_selecionado = rand() % AMOSTRAS;
        float min_distancia = INFINITY;
        float alpha_t, R_t;

        for (int x = 0; x < MAPA_LADO_X; x++)
        {
            for (int y = 0; y < MAPA_LADO_Y; y++)
            {
                float soma_quadrados = 0.0;
                for (int d = 0; d < DIMENSOES; d++)
                {
                    soma_quadrados += pow(dados_entrada[indice_selecionado][d] - mapa[x][y][d], 2);
                }
                float dist = sqrt(soma_quadrados);

                if (dist < min_distancia)
                {
                    min_distancia = dist;
                    v_x = x;
                    v_y = y;
                }
            }
        }

        alpha_t = ALPHA_INICIAL * exp(-(float)t / TEMPO_CONSTANTE);
        R_t = RAIO_INICIAL * exp(-(float)t / TEMPO_CONSTANTE);

        for (int i_x = 0; i_x < MAPA_LADO_X; i_x++)
        {
            for (int i_y = 0; i_y < MAPA_LADO_Y; i_y++)
            {
                float dist_quadrada_topologica = pow((float)i_x - v_x, 2) + pow((float)i_y - v_y, 2);

                float h_ci = 0.0;
                if (R_t <= 0.5)
                    h_ci = (i_x == v_x && i_y == v_y) ? 1.0 : 0.0;
                else
                    h_ci = exp(-dist_quadrada_topologica / (2 * pow(R_t, 2)));

                if (h_ci > 0.001)
                {
                    for (int d = 0; d < DIMENSOES; d++)
                    {
                        float fator_ajuste = alpha_t * h_ci;
                        mapa[i_x][i_y][d] += fator_ajuste * (dados_entrada[indice_selecionado][d] - mapa[i_x][i_y][d]);
                    }
                }
            }
        }
    }

    printf("\n=== MAPA FINAL (Pesos ESPACIALMENTE ORDENADOS) ===\n");

    for (x = 0; x < MAPA_LADO_X; x++)
    {
        for (y = 0; y < MAPA_LADO_Y; y++)
        {
            // Mostrar o primeiro peso (índice 0) de cada neurônio
            printf("%.2f ", mapa[x][y][1]);
        }
        printf("\n");
    }

    fclose(in);
    return 0;
}