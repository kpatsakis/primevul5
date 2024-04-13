static char *get_load(void) {
	static char tmp[64];
	static int count = 0;

	if (count++ % 5 == 0) {
		struct stat sb;
		memset(tmp, 0, sizeof(tmp));
		if (stat("/proc/loadavg", &sb) == 0) {
			int d = open("/proc/loadavg", O_RDONLY);
			if (d >= 0) {
				read(d, tmp, 60);
				close(d);
			}
		}
		if (tmp[0] == '\0') {
			strcat(tmp, "unknown");
		}
	}
	return tmp;
}