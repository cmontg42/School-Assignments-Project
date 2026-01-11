#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    int id;
    int arrival;
    int burst;
    int remaining;
    int wait;
    int turnaround;
} process;

void error_exit(const char *msg) {
	fprintf(stderr, "%s\n", msg);
	exit(1);
}

int done(process *p, int n) {
	for (int i = 0; i < n; i++){
        	if (p[i].remaining > 0){
			return 0;
		}
	}
	return 1;
}

void tick_pr(int t, process *p, int idx) {
	printf("T%d : P%d  - Burst left %2d, Wait time %3d, Turnaround time %3d\n",t, p[idx].id, p[idx].remaining, p[idx].wait, p[idx].turnaround);
}

int fcfs(process *p, int n, int t) {
	int best = -1;
	for (int i = 0; i < n; i++) {
		if (p[i].arrival <= t && p[i].remaining > 0) {
			best = i;
			break;
		}
	
    	}
    	return best;
}

int sjf(process *p, int n, int t, int current_sec) {
	int best = -1;
	int best_rem = 5000;

	for (int i = 0; i < n; i++) {
		if (p[i].arrival <= t && p[i].remaining > 0) {
			if (p[i].remaining < best_rem) {
				best_rem = p[i].remaining;
				best = i;
            		}
        	}
    	}

    // Tie goes to currently running process
    if (best != -1 && current_sec != -1 && p[current_sec].remaining == p[best].remaining)
        return current_sec;

    return best;
}

int main(int argc, char *argv[]) {
    	if (argc != 3 && argc != 4){
        	error_exit("Usage: ./assignment-4 -f|-s|-r <quantum> <filename>");
	}

    	int mode = 0;
    	int quantum = 0;

    	if (strcmp(argv[1], "-f") == 0) {
        	mode = 1;
    	} else if (strcmp(argv[1], "-s") == 0) {
        	mode = 2;
   	} else if (strcmp(argv[1], "-r") == 0) {
        	if (argc != 4){
            		error_exit("Round Robin requires a quantum.");
		}
		quantum = atoi(argv[2]);
		if (quantum <= 0){
            		error_exit("Quantum must be a positive integer.");
		}
		mode = 3;
    	}else {
        	error_exit("Invalid scheduler option.");
    	}
	
    	const char *filename = (mode == 3 ? argv[3] : argv[2]);

	FILE *fp = fopen(filename, "r");
    	printf("file open");
    	if (!fp) error_exit("Could not open input file.");

    	process p[256];
    	int count = 0;
	char name[16];
    	int burst;
    	while (fscanf(fp, "%[^,],%d", name, &burst) == 2 && count < 256){
            	p[count].id = count;
            	p[count].arrival = count;
            	p[count].burst = burst;
            	p[count].remaining = burst;
            	p[count].wait = 0;
            	p[count].turnaround = 0;
            	count++;
    	}
    	fclose(fp);

    	if (count == 0){
        	error_exit("Input file is empty.");
	}
	
    	if (mode == 1) printf("First-Come, First-Served\n");
    	if (mode == 2) printf("Shortest Job First\n");
    	if (mode == 3) printf("Round Robin with Quantum %d\n", quantum);

    	int t = 0;
    	int current_sec = -1;
    	int time_slice = quantum;

    	while (!done(p, count)) {

        	if (mode == 1) current_sec = fcfs(p, count, t);

        	if (mode == 2){
			if(mode == 2 && current_sec == -1){
				current_sec = sjf(p, count, t, -1);
			}else{
				current_sec = sjf(p, count, t, -1);
			}

		}

        	if (mode == 3) {
			if (current_sec == -1 || p[current_sec].remaining == 0 || time_slice == 0) {

                		time_slice = quantum;

                		int found = 0;
                		int start = (current_sec == -1 ? 0 : current_sec + 1);

                		for (int i = 0; i < count; i++) {
                    			int idx = (start + i) % count;
                    			if (p[idx].arrival <= t && p[idx].remaining > 0) {
                        			current_sec = idx;
                        			found = 1;
                        			break;
                    			}
                		}
                		if (!found){
                    			current_sec = -1;
				}
            		}
		

	    		if(current_sec == -1){
				printf("T%d : Idle CPU\n", t);
		    		t++;
		    		continue;
	    		}
		}
        	// Update wait & turnaround times
        	for (int i = 0; i < count; i++) {
            		if (p[i].arrival <= t && p[i].remaining > 0) {
                		p[i].turnaround++;
                		if (i != current_sec){
                    			p[i].wait++;
				}
            		}
        	}

        	if (current_sec != -1) {
            		tick_pr(t, p, current_sec);
            		p[current_sec].remaining--;
        	} else {
            		printf("T%d : Idle CPU\n", t);
        	}

        	if (mode == 3 && current_sec != -1){
            		time_slice--;
		}

        	t++;
    	}

    	printf("\n");

    	double total_wait = 0, total_turnaround = 0;

    	for (int i = 0; i < count; i++) {
        	printf("P%d\n", p[i].id);
        	printf("\tWaiting time:\t\t%5d\n", p[i].wait);
        	printf("\tTurnaround time:\t%5d\n\n", p[i].turnaround);
        	total_wait += p[i].wait;
        	total_turnaround += p[i].turnaround;
    	}

    	printf("Total average waiting time:\t%.1f\n", total_wait / count);
    	printf("Total average turnaround time:\t%.1f\n", total_turnaround / count);

    	return 0;
    
}
