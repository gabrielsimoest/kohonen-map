#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <math.h>
#include <time.h>

#define MAP_WIDTH 10
#define MAP_HEIGHT 10
#define NUM_ATTRIBUTES 14
#define MAX_SAMPLES 178

int main()
{
    // Verificar se a classe deve ser separada do mapa de kohonen mesmo

    int i = 0;
    int j, n;

    float mapa[MAP_HEIGHT][MAP_WIDTH][NUM_ATTRIBUTES];

    // int n,m;
    // int cs;
    // float er;
    // float I0[15],O0[15],I1[15],O1[15],I2[13],O2[13];
    // float  w1[15][14] ,w2[15][13];
    // float nw1[15][14],nw2[15][13];
    // float vw1[15][14],vw2[15][13];
    // float d2[13],d1[14];

    float E1[MAX_SAMPLES], E2[MAX_SAMPLES], E3[MAX_SAMPLES], E4[MAX_SAMPLES], E5[MAX_SAMPLES], E6[MAX_SAMPLES], E7[MAX_SAMPLES], 
    E8[MAX_SAMPLES], E9[MAX_SAMPLES], E10[MAX_SAMPLES], E11[MAX_SAMPLES], E12[MAX_SAMPLES], E13[MAX_SAMPLES], E14[MAX_SAMPLES];

    // LEITURA DOS DADOS
    FILE *in;

    if ((in = fopen("wine.data", "rt")) == NULL)
    {
        printf("Cannot open input file.\n");
        return 1;
    }

    while (!feof(in) && i < 150)
    {
        int result = fscanf(in, "%d,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f",
                            &E1[i], &E2[i], &E3[i], &E4[i], &E5[i], &E6[i],
                            &E7[i], &E8[i], &E9[i], &E10[i], &E11[i], &E12[i], &E13[i], &E14[i]);

        if (result == 14)
        {
            i++;
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

    srand(time(NULL));
    
    for (i = 0; i < MAP_HEIGHT; i++) // linha
    {
        for (j = 0; j < MAP_WIDTH; j++) // coluna
        {
            for (n = 0; n < NUM_ATTRIBUTES; n++) // atributo
            {
                mapa[i][j][n] = (float)rand() / RAND_MAX;
            }
        }
    }
    // Mostrar uma representação visual do mapa
    printf("\n=== MAPA INICIAL (Pesos Aleatórios) ===\n");

    for (i = 0; i < MAP_HEIGHT; i++)
    {
        for (j = 0; j < MAP_WIDTH; j++)
        {
            // Mostrar o primeiro peso (índice 0) de cada neurônio
            printf("%.2f ", mapa[i][j][0]);
        }
        printf("\n");
    }
    
    fclose(in);
    return 0;
}