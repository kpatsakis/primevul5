void send_protected_args(int fd, char *args[])
{
	int i;
#ifdef ICONV_OPTION
	int convert = ic_send != (iconv_t)-1;
	xbuf outbuf, inbuf;

	if (convert)
		alloc_xbuf(&outbuf, 1024);
#endif

	for (i = 0; args[i]; i++) {} /* find first NULL */
	args[i] = "rsync"; /* set a new arg0 */
	if (DEBUG_GTE(CMD, 1))
		print_child_argv("protected args:", args + i + 1);
	do {
		if (!args[i][0])
			write_buf(fd, ".", 2);
#ifdef ICONV_OPTION
		else if (convert) {
			INIT_XBUF_STRLEN(inbuf, args[i]);
			iconvbufs(ic_send, &inbuf, &outbuf,
				  ICB_EXPAND_OUT | ICB_INCLUDE_BAD | ICB_INCLUDE_INCOMPLETE | ICB_INIT);
			outbuf.buf[outbuf.len] = '\0';
			write_buf(fd, outbuf.buf, outbuf.len + 1);
			outbuf.len = 0;
		}
#endif
		else
			write_buf(fd, args[i], strlen(args[i]) + 1);
	} while (args[++i]);
	write_byte(fd, 0);

#ifdef ICONV_OPTION
	if (convert)
		free(outbuf.buf);
#endif
}
