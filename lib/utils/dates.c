#include "dates.h"

long long get_timestamp() {
	struct timespec spec;
	clock_gettime(CLOCK_REALTIME, &spec);

	long long ms = spec.tv_sec;
	ms *= 1000; // Esta originalmente en segundos
	ms += floor(spec.tv_nsec / 1.0e6);

	return ms;
}

long long timestamp_difference(long long initial, long long end) {
	return end > initial ? end - initial : initial - end;
}
