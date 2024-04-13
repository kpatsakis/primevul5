cgets_emu(char *str, int len)
{
    static char buffer[128] = "";
    static int leftover = 0;

    if (buffer[leftover] == NUL) {
	buffer[0] = 126;
	cgets(buffer);
	fputc('\n', stderr);
	if (buffer[2] == 26)
	    return NULL;
	leftover = 2;
    }
    safe_strncpy(str, buffer + leftover, len);
    leftover += strlen(str);
    return str;
}