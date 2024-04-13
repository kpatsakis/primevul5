read_eintr(int fd, void *buf, size_t bufsize)
{
    long ret;

    for (;;)
    {
	ret = vim_read(fd, buf, bufsize);
	if (ret >= 0 || errno != EINTR)
	    break;
    }
    return ret;
}