#include "numbers.h"

int digits_in_a_number(long long number) {
	int totalDigits = 0;

	while (number != 0) {
		number = number / 10;
		totalDigits++;
	}

	return totalDigits;
}


unsigned int division_rounded_up(unsigned int dividend, unsigned int divisor){
    return (dividend + (divisor - 1)) / divisor;
}
