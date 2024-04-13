int html_include(const char *filename)
{
	FILE *f;
	char buf[4096];
	size_t len;

	if (!(f = fopen(filename, "r"))) {
		fprintf(stderr, "[cgit] Failed to include file %s: %s (%d).\n",
			filename, strerror(errno), errno);
		return -1;
	}
	while((len = fread(buf, 1, 4096, f)) > 0)
		write(htmlfd, buf, len);
	fclose(f);
	return 0;
}