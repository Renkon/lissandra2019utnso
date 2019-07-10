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
	return number < 0 ? -number : number;
}

int rnd(int min, int max) {
	if ((max - min) < 0) return -1;

	int divider = (max - min) + 1;
	int random_result = random() % divider;
	return random_result + min;
}

int hash(char* word, int final_value) {
	int sum = 0;
	char* word2 = strdup(word);
	string_to_upper(word2);
	int curr_mult = strlen(word2);
	int curr_char = 0;

	// Funcion de hasheo
	// Agarra cada char y lo multiplica a un numero en base a su posicion y luego le suma la key.
	// Ejemplo: ABCDE:37
	// 65(A) * 5 + 66(B) * 4 + 67(C) * 3 + 68(D) * 2 + 69(E) * 1 + 37

	while (word2[curr_char] != NULL) {
		sum += ((int) word2[curr_char++]) * curr_mult--;
	}

	free(word2);

	log_t("Hash generado usando %s:%i. Resultado: %i", word2, final_value, sum);
    return sum;
}
