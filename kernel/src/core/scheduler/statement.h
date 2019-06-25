#ifndef CORE_SCHEDULER_STATEMENT_H_
#define CORE_SCHEDULER_STATEMENT_H_

typedef struct {
	operation_t operation;
	select_input_t* select_input;
	insert_input_t* insert_input;
	create_input_t* create_input;
	describe_input_t* describe_input;
	drop_input_t* drop_input;
	sem_t* semaphore;
} statement_t;

#endif /* CORE_SCHEDULER_STATEMENT_H_ */
