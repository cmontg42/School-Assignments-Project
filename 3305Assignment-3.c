#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int rowRes[9], colRes[9], gridRes[9];
typedef struct {
	int row;
	int col;
	int subGrid;
	int *grid;
}ThreadArgs;

void *rowWork(void *arg){
	ThreadArgs *rowPointer = arg;
	int rowIsValid = 0;
	int row[9];
	int rowToCheck = rowPointer->row;
	int *sudokuArray = rowPointer->grid;
	int n = (rowToCheck - 1);
	int rowVal = 0;
	for(int i = (0 + 9*n); i < (9 * rowToCheck); i++){
		row[rowVal] = sudokuArray[i];
		rowVal++;
	}

	for(int k = 0; k < 9; k++){
		for(int j = 0; j < 9; j++){
			if ((row[k] == row[j]) && (k != j)){
				rowIsValid = 1;
			}
		}
	}
	rowRes[rowToCheck-1] = rowIsValid;

}
void *colWork(void *arg){
	ThreadArgs *colPointer = arg;
	int colIsValid = 0;
	int col[9];
	int colToCheck = colPointer->col;
	int *sudokuArray = colPointer->grid;
	int n = (colToCheck - 1);
	int colVal = 0;
	for(int i = (0 + n); i < 81; i+=9){
		col[colVal] = sudokuArray[i];
		colVal++;
	}

	for(int k = 0; k < 9; k++){
		for(int j = 0; j < 9; j++){
			if ((col[k] == col[j]) && (k != j)){
				colIsValid = 1;
			}
		}
	}
	colRes[colToCheck-1] = colIsValid;
}


void *gridWork(void *arg){
	ThreadArgs *gridPointer = arg;
	int gridIsValid = 0;
	int grid[9];
	int row1[9], row2[9], row3[9];
	int layer = 0;
	int subGrid = gridPointer->subGrid;
	int *sudokuArray = gridPointer->grid;
	if(subGrid < 4){
		layer = 1;
		for(int i = 0; i < 27; i++){
			if(i < 9){
				row1[i] = sudokuArray[i];
			}else if(i > 8 && i < 18){
				row2[i-9] = sudokuArray[i];
			}else if(i > 17 && i < 27){
				row3[i-18] = sudokuArray[i];
			}
		}
	}else if(subGrid > 3 && subGrid < 7){
		layer = 2;
		 for(int i = 27; i < 54; i++){
                        if(i > 26 && i < 36){
                                row1[i-27] = sudokuArray[i];
                        }else if(i > 35 && i < 45){
                                row2[i-36] = sudokuArray[i];
                        }else if(i > 44 && i < 54){
                                row3[i-45] = sudokuArray[i];
                        }
                }
	}else if(subGrid > 6 && subGrid < 10){
		layer = 3;
		 for(int i = 54; i < 81; i++){
                        if(i > 53 && i < 63){
                                row1[i-54] = sudokuArray[i];
                        }else if(i > 62 && i < 72){
                                row2[i-63] = sudokuArray[i];
                        }else if(i > 71 && i < 81){
                                row3[i-72] = sudokuArray[i];
                        }
                }
	}


	if((subGrid == 1)||(subGrid == 4)||(subGrid == 7)){
		for(int i = 0; i < 3; i++){
			grid[i] = row1[i];
			grid[i+3] = row2[i];
			grid[i+6] = row3[i];
		}
	}else if((subGrid == 2)||(subGrid == 5)||(subGrid == 8)){
                for(int i = 0; i < 3; i++){
                        grid[i] = row1[i+3];
                        grid[i+3] = row2[i+3];
                        grid[i+6] = row3[i+3];
                }
        }else if((subGrid == 3)||(subGrid == 6)||(subGrid == 9)){
                for(int i = 0; i < 3; i++){
                        grid[i] = row1[i+6];
                        grid[i+3] = row2[i+6];
                        grid[i+6] = row3[i+6];
                }
        }



	 for(int k = 0; k < 9; k++){
                for(int j = 0; j < 9; j++){
                        if ((grid[k] == grid[j]) && (k != j)){
                                gridIsValid = 1;
                        }
                }
        }

	gridRes[subGrid-1] = gridIsValid;
}



int main(int argc, char *argv[]) {
	memset(rowRes, 0, sizeof(rowRes));
	memset(colRes, 0, sizeof(colRes));
	memset(gridRes, 0, sizeof(gridRes));
        FILE *sudokuF;
	int sudoku[81];
	if (argc < 2){
		return 0;
	}
	

	pthread_t threadRows[9];
	pthread_t threadCols[9];
	pthread_t threadGrids[9];


	sudokuF = fopen(argv[1], "r");
	for (int j = 0; j < 81; j++){
		if(fscanf(sudokuF, "%d", &sudoku[j]) != 1){
			printf("Error at %d\n", j);
			fclose(sudokuF);
			return 0;
		}
	}
	fclose(sudokuF);


	ThreadArgs *rows[9]; 
	for(int i = 1; i < 10; i++){
		rows[i-1] = malloc(sizeof(ThreadArgs));
		rows[i-1]->row = i;
		rows[i-1]->col = 0;
		rows[i-1]->subGrid = 0;
		rows[i-1]->grid = sudoku;
		pthread_create(&threadRows[i-1], NULL, rowWork, rows[i-1]);
	}


	int awwMan = 0;
	for(int k = 0; k < 9; k++){
		pthread_join(threadRows[k], NULL);
		free(rows[k]);
		if(rowRes[k] == 1){
			printf("Thread #%d (row %d) is invalid\n\n", k, k);
			awwMan += 1;
		}else{
			printf("Thread #%d (row %d) is valid\n\n", k, k);
		}
	}


	ThreadArgs *cols[9];
        for(int c = 1; c < 10; c++){
                cols[c-1] = malloc(sizeof(ThreadArgs));
                cols[c-1]->row = 0;
                cols[c-1]->col = c;
                cols[c-1]->subGrid = 0;
                cols[c-1]->grid = sudoku;
                pthread_create(&threadCols[c-1], NULL, colWork, cols[c-1]);
        }
 

        for(int d = 0; d < 9; d++){
                pthread_join(threadCols[d], NULL);
                free(cols[d]);
                if(colRes[d] == 1){
                        printf("Thread #%d (col %d) is invalid\n\n", d+9, d);
			awwMan += 1;
                }else{
                        printf("Thread #%d (col %d) is valid\n\n", d+9, d);
                }
        }
	

	ThreadArgs *grids[9];
        for(int g = 1; g < 10; g++){
                grids[g-1] = malloc(sizeof(ThreadArgs));
                grids[g-1]->row = 0;
                grids[g-1]->col = 0;
                grids[g-1]->subGrid = g;
                grids[g-1]->grid = sudoku;
                pthread_create(&threadGrids[g-1], NULL, gridWork, grids[g-1]);
        }


        for(int f = 0; f < 9; f++){
                pthread_join(threadGrids[f], NULL);
                free(grids[f]);
                if(gridRes[f] == 1){
                        printf("Thread #%d (grid %d) is invalid\n\n", f+18, f);
			awwMan += 1;
                }else{
                        printf("Thread #%d (grid %d) is valid\n\n", f+18, f);
                }
        }

	if(awwMan == 0){
		printf("%s contains a VALID sudoku solution\n", argv[1]);
	}else{
		printf("%s contains an INVALID solution\n", argv[1]);
	}

		
}
