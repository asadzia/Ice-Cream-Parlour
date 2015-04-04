/*
* Created by: Asad Zia
* 
* Description:
* An ice cream parlour simulation in C. The simulation delineates an ice-cream parlour with two tables and 8 seats (4 on each table).
* If a customer arrives then he can sit down on a partially filled table but in case a table is full and someone leaves that table, then
* the customer has to wait for the entire table to be empty to take a seat.
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

#define NUMBER_OF_CUSTOMERS 11
#define NUMBER_OF_TABLES 2

sem_t queue_mutex[NUMBER_OF_TABLES];			/* a mutex variable used for controlling customers on the tables */
sem_t table_sem;								/* the semaphore which holds the capacity of the total people allowed in the shop, i.e 8 */
sem_t queue_sem[NUMBER_OF_TABLES];				/* the semaphore used to describe how only four people can sit at one table */
sem_t conditional[NUMBER_OF_TABLES];			/* a sempahore used to control the flow of customers in each table */

int count = 0;									/* this is a counter which counts the executions of threads */

/*
* A data-structure used to model the airplanes
*/
typedef struct {
	int id; 		/* The ID of the customer */
	pthread_t tid; 	/* The thread ID of the customer */
} customer;

/*
* The function which checks if a table is available in the ice-cream shop
*/
static void check_for_table(int id) {
	printf("Customer %d checks for a table!\n", id);
	sem_wait(&table_sem);	/* a semaphore of capacity 8 which checks if there is space available in the shop */
}

/*
* This function allows a customer to occupy a table. This is done by using multiple semaphores.
* It should be noted that I have implemnted two ways for customers to leave the ice-cream shop.
* The first way of leaving is that all 4 people sitting on a table leave together.
* The second way is that a customer comes and sits on a vacant table and then leaves.
*/
static void occupy_table(int id) {

	int num = NUMBER_OF_CUSTOMERS % 4;									/* finding those customers who will come and leave individually */
	count++;															/* this counter counts the number of customers (threads) coming in */
	int sval, sval_one, sval_two, val_one, val_two, val_three, val_four;

	sem_getvalue(&queue_sem[0], &val_one);								/* getting the value of the number of customers on table 1 */
	sem_getvalue(&queue_sem[1], &val_two);								/* getting the value of the number of customers on table 2 */
	sem_getvalue(&conditional[0], &val_three);							/* getting the value of the variable controlling entry on table 1 */
	sem_getvalue(&conditional[1], &val_four);							/* getting the value of the variable controlling entry on table 2 */


    if (val_three == 1 && val_one > 0) {								/* a conditional statement for the first table */
    	if ((NUMBER_OF_CUSTOMERS - count) < 4 && NUMBER_OF_CUSTOMERS - num  < count) {	/* a statement for customers to leave individually */
    		sem_wait(&queue_sem[0]);									/* decrement semaphore to signify that customer sits on the table */
	 	   	printf("Customer %d sits on table 1!\n", id);
	    	sem_getvalue(&queue_sem[0], &sval);						
	        sem_post(&queue_sem[0]);									/* increment to signify that customer leaves table */
	    	sem_getvalue(&conditional[0], &val_three);
	  	    sem_getvalue(&queue_sem[0], &sval_one);

		 	printf("Customer %d leaves table 1\n", id);   
		    sleep(2);													
	    	if (val_three == 0 && sval_one == 4 && val_three < 2) {		/* change the conditional sempahore back to to make this if-statement usable by another thread */
	    		sem_post(&conditional[0]);
	    	} 
	    	sem_post(&table_sem);										/* increment the table sempahore to signal an open place on the table */
	    	goto end;													/* goto used to avoid the coming lines of code */
    	}	

    	/* this section deals with all 4 customers leaving together from a table */
		sem_wait(&queue_sem[0]);										/* decrement semaphore queue for a table */
	    printf("Customer %d sits on table 1!\n", id);
	    sem_getvalue(&queue_sem[0], &sval);			
	    if (sval != 0) {
	        sem_wait(&queue_mutex[0]);			/* enter critical section if there are still spaces left on the table */
	    } else {
	    	sem_wait(&conditional[0]);			/* when fourth customer sits on table, decrease the conditonal semaphore to prevent another customer from approaching the table */
	        sem_post(&queue_mutex[0]);			/* allow the blocked threads to leave critical section to show that the customers have eaten */	
	        sem_post(&queue_mutex[0]);			/* and are prepared to leave */
	        sem_post(&queue_mutex[0]);
	    }
	    sem_post(&queue_sem[0]);				/* each thread increments its value on the queue semaphore back to 4 to indicate that table is empty */
	    sem_getvalue(&conditional[0], &val_three);
	    sem_getvalue(&queue_sem[0], &sval_one);
	 	printf("Customer %d leaves table 1!\n", id);   
	    sleep(5);
	    if (val_three == 0 && sval_one == 4 && val_three < 2) {	/* change the conditional sempahore back to to make this if-statement usable by another thread */
	    	sem_post(&conditional[0]);
	    } 
	    sem_post(&table_sem);					/* increment the table sempahore to signal an open place on the table */		
	    goto end;								/* goto used to avoid the coming lines of code */

	}

    if (val_four == 1 && val_two > 0) {													/* a conditional statement for the second table */
    	if ((NUMBER_OF_CUSTOMERS - count) < 4 && NUMBER_OF_CUSTOMERS - num < count) {	/* a statement for customers to leave individually */
    		sem_wait(&queue_sem[1]);									/* decrement semaphore to signify that customer sits on the table */
	 	   	printf("Customer %d sits on table 2!\n", id);
	    	sem_getvalue(&queue_sem[1], &sval);
	        sem_post(&queue_sem[1]);									/* increment to signify that customer leaves table */
	    	sem_getvalue(&conditional[1], &val_four);
	  	    sem_getvalue(&queue_sem[1], &sval_two);

		 	printf("Customer %d leaves table 2!\n", id);   
		    sleep(5);
	    	if (val_four == 0 && sval_two == 4 && val_four < 2) {		/* change the conditional sempahore back to to make this if-statement usable by another thread */
	    		sem_post(&conditional[1]);
	    	} 
	    	sem_post(&table_sem);										/* increment the table sempahore to signal an open place on the table */
	    	goto end;													/* goto used to avoid the coming lines of code */
    	}

    	/* this section deals with all 4 customers leaving together from a table */
		sem_wait(&queue_sem[1]);										/* decrement semaphore queue for a table */
	    printf("Customer %d sits on table 2!\n", id);
	    sem_getvalue(&queue_sem[1], &sval);
	    if (sval != 0) {
	        sem_wait(&queue_mutex[1]);								/* enter critical section if there are still spaces left on the table */
	    }else {
	    	sem_wait(&conditional[1]);		/* when fourth customer sits on table, decrease the conditonal semaphore to prevent another customer from approaching the table */
	        sem_post(&queue_mutex[1]);
	        sem_post(&queue_mutex[1]);		/* allow the blocked threads to leave critical section to show that the customers have eaten */	
	        sem_post(&queue_mutex[1]);		/* and are preapred to leave */
	    }
	    sem_post(&queue_sem[1]);			/* indicate that place is empty in a table */
	    sem_getvalue(&conditional[1], &val_four);
	    sem_getvalue(&queue_sem[1], &sval_two);
	    
	    if (val_four == 0 && sval_two == 4 && val_four < 2) {	/* change the conditional sempahore back to to make this if-statement usable by another thread */
	    	sem_post(&conditional[1]);
	    } 
	 	printf("Customer %d leaves table 2!\n", id);  
	    sleep(2);
	 	sem_post(&table_sem); 									/* increment the table sempahore to signal an open place on the table */
	}
	end:;		/* the jump done from the goto statement reaches the end of the function which does nothing */
}

/* 
* a function which exits the program one all customers have enterted the shop and are finished 
*/
static void counter(int id) {
	if (count == NUMBER_OF_CUSTOMERS) {
		exit(1);
	}
}

/* 
* the basic thread fucntion which calls all the helper functions */
static void* customer_control(void* args)
{
    customer* customers = (customer*)args;
    check_for_table(customers->id);
    occupy_table(customers->id);
    counter(customers->id);
    return NULL;
}


/*
* the main function
*/
int main () {
	int i, status;
	customer* customers;

	 /* creating semaphore for allowing entry into the shop. this should have a value of 8 to indicate the limit of 8 customers at one time */
	 if (sem_init(&table_sem, 0, 8)) {
        fprintf(stderr, "Initialization of semaphores failed\n");
        exit(EXIT_FAILURE);
    }

    /* creating other semaphores. The notable semaphore here is queue_sem which has a value of 4 to indicate 4 places on one table */
    for (i = 0; i < NUMBER_OF_TABLES; i++) {
    	 if (sem_init(&queue_sem[i], 0, 4) || sem_init(&queue_mutex[i], 0, 0) || sem_init(&conditional[i], 0, 1)) {
    	 	fprintf(stderr, "Initialization of semaphores failed\n");
       		 exit(EXIT_FAILURE);
    	 }
    }

	 // allocate memory for the customers data structure
	customers = (customer *)calloc(NUMBER_OF_CUSTOMERS, sizeof(customer));
	if (!customers) {
		printf("Error in memory allocation!\n");
		exit(1);
	}

	// creating the threads
	for (i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
		customers[i].id = i + 1;
		status = pthread_create(&customers[i].tid, NULL, customer_control, (void*)&customers[i]);

		if (status != 0) {
			printf("Error in function pthread_create()!\n");
		}
	}

	// joining the threads
	for (i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
		pthread_join( customers[i].tid, NULL);
	}

	// freeing the memory for the plane data-structure
	(void) free(customers);


	/* destroy the semaphores */
	if (sem_destroy(&table_sem)) {
        fprintf(stderr, "Destruction of semaphores failed!\n");
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < NUMBER_OF_TABLES; i++) {
   		if (sem_destroy(&queue_sem[i]) || sem_destroy(&queue_mutex[i]) || sem_destroy(&conditional[i])) {
   			 fprintf(stderr, "Destruction of semaphores failed!\n");
      		 exit(EXIT_FAILURE);
   		}
    }
	return 0;
}
