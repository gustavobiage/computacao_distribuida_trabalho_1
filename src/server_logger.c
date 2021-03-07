void * concurrent_log(void * par) {
	while (1) {
		sleep(SLEEP_AMOUNT * SLEEP_UNIT);
		// TODO
	}
	pthread_exit(0);
}
