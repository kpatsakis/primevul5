nameOf (char* name, int cnt)
{
    static char* buf = 0;

    if (*name == '\0')
	return name;
    if (cnt) {
	if (!buf)
	    buf = N_NEW (strlen(name)+32,char);  /* 32 to handle any integer plus null byte */
	sprintf (buf, "%s%d", name, cnt);
	return buf;
    }
    else
	return name;
}