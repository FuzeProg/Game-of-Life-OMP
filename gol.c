//
// gol.c
// The objective is to implement openMP in this programm to know how it work.
// Source code : https://rosettacode.org/wiki/Conway%27s_Game_of_Life/C
// Source OpenMP functions : https://www.openmp.org/wp-content/uploads/OpenMP-4.0-C.pdf
// Edited by : MARECHAL Anthony - MOZDZIERZ Ombeline
//
// Rule 1 : 3 alive cells around a dead one make it alive (she born)
// Rule 2 : An alive cell which have 2 or 3 alive cells stay alive
// Rule 3 : An alive cell which have more than 3 alive cells or less than 2 alive cells die
// Restriction : The game will be stopped if a board is equal to the next 2 times in a row, or when all of the rounds are played.
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

/* ------------------- START - DEFINITIONS ------------------- */

// Define a lambda cellule
#define CELL(I, J) (field[size * (I) + (J)])
// Define an alive cellule
#define ALIVE(I, J) t[size * (I) + (J)] = 1
// Define a dead cellule
#define DEAD(I, J) t[size * (I) + (J)] = 0
// Define an initial cellule at the generation of the board
#define ACTIVE_CELL(I, J) field[BOARD_SIZE * (I) + (J)] = 1
// Define the size of the board
#define BOARD_SIZE 14
// Define the number of rounds that will be played
#define ROUNDS 10000000
// Define the threads number
#define NBTHREADS 2

/* ------------------- END - DEFINITIONS ------------------- */

/* ------------------- START - FUNCTIONS ------------------- */

// Alive counter that will return the number of alive cells around one
int cpt_alive(const char *field, int a, int b, int size) {
    int i, j, k = 0;

    for (i = a - 1; i <= (a + 1); i++) {
        for (j = b - 1; j <= (b + 1); j++) {
            if ((i == a) && (j == b))
                continue;
            if ((j < size) && (i < size) && (i >= 0) && (j >= 0))
                a += CELL(i, j);
        }
    }
    return k;
}

// Return a random number between min and max
int randInRange(unsigned int min, unsigned int max){
    return rand() % (max + 1 - min) + min;
}

// Play the game, evolve the cells and manage which one will be dead or alive dependent of the rules
void evolve(const char *field, char *t, int size) {
    unsigned int i, j, alive, cs;

    // i, j, alive and cs will be private to a thread
#pragma omp parallel private (i, j, alive, cs)
    for (i = 0; i < size; i++) {

        /*
         * Iterations will be executed in parallel from threads.
         * Basically the iteration variable will be private.
         */
#pragma omp for
        for (j = 0; j < size; j++) {
            alive = cpt_alive(field, i, j, size);
            cs = CELL(i, j);

            // Loop that will define if a cell is alive or dead
            if (cs) {
                if ((alive > 3) || (alive < 2))
                    DEAD(i, j);
                else
                    ALIVE(i, j);
            } else {
                if (alive == 3)
                    ALIVE(i, j);
                else
                    DEAD(i, j);
            }
        }
    }
}

/*
 * If there is an f[i], it will take 'X' value, else it will take '.' value.
 * 'X' is an alive cell
 * '.' is a dead cell
 */
void dump_field(const char *f, int size) {
    int i;

    for (i = 0; i < (size * size); i++) {
        if ((i % size) == 0)
            printf("\n");
        /*
         * Ternary operator
         * (if) [cond] ? [true] : [false]
         */
        printf("%c", f[i] ? 'X' : '.');
    }

    printf("\n");
}

/* ------------------- END - FUNCTIONS ------------------- */

/* ------------------- START - MAIN ------------------- */

int main(int argc, char **argv) {

    // Lambda variables
    int i, compt;
    // Board, next board, transitive board
    char *fa, *fb, *tt;
    // Time at the start and the end of the execution
    double start, finish;
    // Initialization variables
    int random, a, b;

    srand(time(NULL));

    // Setting up the threads number
    omp_set_num_threads(NBTHREADS);

    char field[BOARD_SIZE * BOARD_SIZE];
    char tmp_field[BOARD_SIZE * BOARD_SIZE];
    int size = BOARD_SIZE * BOARD_SIZE;

    for (i = 0; i < size; i++)
        field[i] = 0;

    // Initialize the game with random method
    for(i = 0; i < BOARD_SIZE*BOARD_SIZE; i++){
        random = randInRange(0, 1);
        if(random == 0){
            a = randInRange(0, BOARD_SIZE);
            b = randInRange(0, BOARD_SIZE);
            ACTIVE_CELL(a, b);
        }
    }

    fa = field;
    fb = tmp_field;

    printf("Running with %d threads.\nFirst gen :\n\n", NBTHREADS);
    dump_field(fa, BOARD_SIZE);

    start = omp_get_wtime();

    for(i = 0; i < ROUNDS; i++){
        if(compt < 2) {
            evolve(fa, fb, BOARD_SIZE);
            tt = fb;
            fb = fa;
            fa = tt;
            if(fa == fb ){
                compt = compt+1;
            }
            else
                compt = 0;
        } else{
            printf("Le tablezu n'a pas bouger depuis 2 tours\n");
            break;
        }
    }

    finish = omp_get_wtime();
    printf("DONE. Last gen :\n");
    dump_field(fa, BOARD_SIZE);
    printf("\nElapsed wall clock time = %f seconds.\n", finish - start);
    printf("\nTerminé !\n\n");

    return 0;

}

/* ------------------- END - MAIN ------------------- */
