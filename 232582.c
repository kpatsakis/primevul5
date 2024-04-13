write_eintr(int fd, void *buf, size_t bufsize)
{
    long    ret = 0;
    long    wlen;

    /* Repeat the write() so long it didn't fail, other than being interrupted
     * by a signal. */
    while (ret < (long)bufsize)
    {
	wlen = vim_write(fd, (char *)buf + ret, bufsize - ret);
	if (wlen < 0)
	{
	    if (errno != EINTR)
		break;
	}
	else
	    ret += wlen;
    }
    return ret;
}