#include <stdio.h>
#include <pthread.h>
#include "easy_log.h"

static void bt() {
	EASY_PRINT_BT("trace");
}

int main() {
	easy_warn_log("old level: %d", easy_log_level);
	easy_log_level = EASY_LOG_ALL;
	easy_trace_log("for tracing\n");
	easy_fatal_log("there are some errors\n");
	easy_log_print_default("for test in default\n");
	bt();
	return 0;
}
