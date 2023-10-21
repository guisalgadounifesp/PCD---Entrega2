// Guilherme Salgado Alves - RA: 140083
// Lucas Gabriel Camargo - RA: 140449

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define N 2048
#define N_THREADS 2

void FillGlider(int **grid)
{
    // GLIDER
    int lin = 1, col = 1;

    grid[lin][col + 1] = 1;
    grid[lin + 1][col + 2] = 1;
    grid[lin + 2][col] = 1;
    grid[lin + 2][col + 1] = 1;
    grid[lin + 2][col + 2] = 1;
}

void FillRPentonimo(int **grid)
{
    // R-pentomino
    int lin = 10;
    int col = 30;
    grid[lin][col + 1] = 1;
    grid[lin][col + 2] = 1;
    grid[lin + 1][col] = 1;
    grid[lin + 1][col + 1] = 1;
    grid[lin + 2][col + 1] = 1;
}

void PrintGrid(int **grid)
{
    int i, j;
    char gridcell;
    for (i = 0; i < 50; i++)
    {
        for (j = 0; j < 50; j++)
        {
            if(grid[i][j]==1){
                gridcell = '*';
            }else{
                gridcell = '.';
            }
            printf("%c", gridcell);
        }

        printf("\n");
    }
    printf("\n\n\n");
}

int GetVizinhosVivos(int **grid, int line, int column)
{
    int vivo = 0;
    int j = 0;
    int atual = 0;
    int acima = 0;

    // Pega linha de cima
    if (line == 0) {
        acima = N - 1;
    }else{
        acima = line - 1;
    }

    /* Percorre as 3 células vizinhas na linha de cima, 1 imediatamente acima e 2 diagonais */
    for (j = column - 1; j <= column + 1; j++)
    {
        // Checa borda infinita
        atual = j;
        if (atual < 0)
            atual = N - 1;

        if (atual > N - 1)
            atual = 0;

        if (grid[acima][atual] == 1)
            vivo++;
    }

    // Pega linha de baixo
    int abaixo = (line + 1) % N;

    /* Percorre as 3 células vizinhas na linha de baixo, 1 imediatamente abaixo e 2 diagonais */
    for (j = column - 1; j <= column + 1; j++)
    {
        // Checa borda infinita
        atual = j;
        if (atual < 0)
            atual = N - 1;

        if (atual > N - 1)
            atual = 0;

        if (grid[abaixo][atual] == 1)
            vivo++;
    }

   // Checa se célula à esquerda está viva
     int Esq = 0;
     if (column > 0) {
        Esq = column -1;
     }else{
        Esq = N - 1;
     }
    if (grid[line][Esq] == 1)
        vivo++;

    // Checa se célula à direita está viva
    int Dir = 0;
    if ( column < N - 1){
        Dir = column + 1;
    }else{
        Dir = 0;
    }
    if (grid[line][Dir] == 1)
        vivo++;

    return vivo;
}

int GetNovoEstado(int **grid, int line, int column)
{
    int Vizinhos = GetVizinhosVivos(grid, line, column);

    // Verifica se a célula está viva, se grid = 1
    if (grid[line][column] == 1)
    {
        // Se há 2 ou 3 vizinhos vivos, mantém-se viva
        if (Vizinhos == 2 || Vizinhos == 3)
            return 1;

        // Caso contrário, morre
        return 0;
    }

    /* Caso a célula não esteja viva, verifica se há vizinhos 
    suficientes para viver, ou seja, 3 */
    if (Vizinhos == 3)
        return 1;

    return 0;
}


int **GetProximoGrid(int **gridA, int **gridB, int iteracao)
{
    if (iteracao % 2 == 0)
        return gridB;
    else
        return gridA;
}

int **GetGridAtual(int **gridA, int **gridB, int iteracao)
{
    if (iteracao % 2 == 0)
        return gridA;
    else
        return gridB;
}

void PlayJogoDaVida(int **gridA, int **gridB, int iteracao)
{
    int i, j, k;
    int th_id;
    int print_thread_num = 0;

    for (k = 0; k < iteracao; k++)
    {
        int **ProximoGrid = GetProximoGrid(gridA, gridB, k);
        int **GridAtual = GetGridAtual(gridA, gridB, k);

        /*if (k < 5)
            PrintGrid(GridAtual);*/

#pragma omp parallel default(none) shared(ProximoGrid, GridAtual, print_thread_num) private(i, j, th_id)
        {
            th_id = omp_get_thread_num();
            if (th_id == 0 && print_thread_num == 0)
            {
                printf("Numero de threads: %d\n\n", omp_get_num_threads());
                print_thread_num = 1;
            }

#pragma omp for
            for (i = 0; i < N; i++)
            {
                for (j = 0; j < N; j++)
                {
                    ProximoGrid[i][j] = GetNovoEstado(GridAtual, i, j);
                }
            }
        }
    }
}

int GetSobreviventes(int **grid)
{
    int vivo = 0;
    int i, j;
    #pragma omp parallel default(none) shared(vivo,grid) private(i, j)
    
    
    #pragma omp for
    for (i = 0; i < N; i++)
    {
        for (j = 0; j < N; j++)
        {
        #pragma omp critical (CriticalSum)
            vivo += grid[i][j];
        }
    }
    return vivo;
}


int main()
{
    int **gridA, **gridB;
    gridA = (int **)malloc(N * sizeof(int *));
    gridB = (int **)malloc(N * sizeof(int *));

    double start, end;

    int i = 0, j = 0;
    for (i = 0; i < N; i++)
    {
        gridA[i] = (int *)malloc(N * sizeof(int));
        gridB[i] = (int *)malloc(N * sizeof(int));

        for (j = 0; j < N; j++)
        {
            gridA[i][j] = 0;
            gridB[i][j] = 0;
        }
    }

    FillGlider(gridA);
    FillRPentonimo(gridA);
    omp_set_num_threads(N_THREADS);
    printf("Condicao inicial: %d\n", GetSobreviventes(gridA));

    start = omp_get_wtime();
    PlayJogoDaVida(gridA, gridB, 2001);
    end = omp_get_wtime();
    
    double survivorStart, survivorEnd;
    
    survivorStart = omp_get_wtime();
    
    printf("Ultima geracao (2000 iteracoes): %d\n", GetSobreviventes(gridB));
    
    survivorEnd = omp_get_wtime();
    
    printf("Tempo execucao trecho de teste: %f\n", survivorEnd - survivorStart);
    printf("Tempo execucao: %f\n", end - start);

    return 0;
}
