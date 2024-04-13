static inline int strtaglen(const char *str, rpm_count_t c, const char *end)
{
    const char *start = str;
    const char *s = NULL;
    int len = -1; /* assume failure */

    if (end) {
	while (end > start && (s = memchr(start, '\0', end-start))) {
	    if (--c == 0)
		break;
	    start = s + 1;
	}
    } else {
	while ((s = strchr(start, '\0'))) {
	    if (--c == 0)
		break;
	    start = s + 1;
	}
    }

    if (s != NULL && c == 0)
	len = s - str + 1;

    return len;
}