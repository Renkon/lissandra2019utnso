#ifndef UTILS_NUMBERS_H_
#define UTILS_NUMBERS_H_
#include <stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "commons/string.h"
#include "../generic_logger.h"

int digits_in_a_number(long long number);
unsigned int division_rounded_up(unsigned int dividend, unsigned int divisor);
int abs(int number);
int rnd(int min, int max);
int hash(char* word, int final_value);

#endif /* UTILS_NUMBERS_H_ */
