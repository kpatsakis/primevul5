defaultNoticeProcessor(void *arg, const char *message)
{
	(void) arg;					/* not used */
	/* Note: we expect the supplied string to end with a newline already. */
	fprintf(stderr, "%s", message);
}