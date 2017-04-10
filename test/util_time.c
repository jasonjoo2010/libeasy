#include <stdio.h>
#include <easy_time.h>

int main() {
	struct tm tm;
	int64_t now = easy_time_now();
	printf("now: %lld\n", now);
	//us => s
	now /= 1000000;
	easy_localtime((time_t *)&now, &tm);
	printf("local: %d-%d-%d %d:%d:%d\n", tm.tm_year + 1900, tm.tm_mon, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
	return 0;
}
