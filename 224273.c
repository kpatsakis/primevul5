static char *host_end(char **hoststart, int removebrackets)
{
	char *host = *hoststart;
	char *end;
	char *start = strstr(host, "@[");
	if (start)
		start++; /* Jump over '@' */
	else
		start = host;
	if (start[0] == '[') {
		end = strchr(start + 1, ']');
		if (end) {
			if (removebrackets) {
				*end = 0;
				memmove(start, start + 1, end - start);
				end++;
			}
		} else
			end = host;
	} else
		end = host;
	return end;
}