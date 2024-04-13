void nap_sleep(int ms, int split) {
	int i, input = got_user_input;
	int gd = got_local_pointer_input;

	for (i=0; i<split; i++) {
		usleep(ms * 1000 / split);
		if (! use_threads && i != split - 1) {
			rfbPE(-1);
		}
		if (input != got_user_input) {
			break;
		}
		if (gd != got_local_pointer_input) {
			break;
		}
	}
}