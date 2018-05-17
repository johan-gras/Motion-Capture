#include <stdio.h>
#include <sys/time.h>
#include <linux/types.h>
#include <signal.h>
#include <stdlib.h>

#include "acc.h"
#include "network.h"


#define TASK_PERIOD_RESET 200000
#define TASK_DEADLINE 12


// Accelerometer
int x, y, x_acc, y_acc;
// Adaptative task period
enum mode {droping, testing, climbing, stable};
int task_period = TASK_PERIOD_RESET;
int count_ada = 1, mode = droping;
int mesure_ada = 0, manque_ada = 0;
int mesure = 0, manque = 0;

/* Signal Handler for SIGINT */
void sigintHandler(int sig_num)
{
    printf("\nMesure : %d\n", mesure);
    printf("Manque : %d\n", manque);
    printf("Ratio manque : %f\n", ((double)manque / (double)mesure)*100);
    fflush(stdout);
    exit(0);
}

// Periodic task
void task(){
    get_pos(&x, &y, &x_acc, &y_acc);
    send_data(&x, &y, &x_acc, &y_acc);
}

// Optimize the task period
void adaptative_task_period(){
    count_ada--;

    if (count_ada == 0){
        printf("\nTask period : %d\n", task_period);
        printf("Mesure : %d    Manque : %d\n", mesure_ada, manque_ada);

        // Drop the period until a missing append
        if (mode == droping){
            if (manque_ada == 0){
                task_period *= 0.95;
                count_ada = 1;
            }
            else{
                count_ada = 10;
                mode = testing;
            }
        }

        // Test if the missing was an exeption
        else if (mode == testing){
            if (manque_ada == 0){
                task_period *= 0.95;
                count_ada = 1;
                mode = droping;
            }
            else{
                task_period *= 1.01;
                count_ada = 30;
                mode = climbing;
            }
        }

        // Climb back the period task until no missing append anymore
        else if (mode == climbing){
            if (manque_ada == 0){
                count_ada = 100;
                mode = stable;
                printf("Task period stabilized at %d.\n", task_period);
            }
            else{
                task_period *= 1 + 0.005 * manque_ada;
                count_ada = 30;
            }
        }

        // Keep the same task period while still watching missings
        else{
            if (manque_ada > 2)
                task_period *= 1.01;

            count_ada = 100;
            mesure += mesure_ada;
            manque += manque_ada;
        }

        mesure_ada = 0;
        manque_ada = 0;
    }
}


int main()
{
    struct timeval time;
    long int time_diff, time_next;
    signal(SIGINT, sigintHandler);

    init_acc();
    init_network();
    printf ("Starting...\n");
    gettimeofday(&time, 0);
    time_next = time.tv_sec * 1000000L + time.tv_usec + task_period;


    // Timing gestion
    while(1){
        gettimeofday(&time, 0);
        time_diff = time.tv_sec * 1000000L + time.tv_usec - time_next;

        // Deadline
        if (time_diff >= 0) {
            mesure_ada++;

            // Failed
            if (time_diff >= task_period / TASK_DEADLINE){
                //fprintf (stderr,"***echeance manquee \n");
                //fprintf(stderr,"temps écoule=%ld usec\n", time_diff);
                manque_ada++;
            }
            // In time
            else{
                task();
            }

            // Adaptative task period
            adaptative_task_period();
            time_next += task_period;
        }
    }
    
}
