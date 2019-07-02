#include "numbers.h"

int digits_in_a_number(long long number) {
	if (number == 0) return 1;

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

int abs(int number) {

	if (number < 0) {
		return -number;

	} else {
		return number;
	}

}
