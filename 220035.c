int headerWrite(FD_t fd, Header h, int magicp)
{
    ssize_t nb;
    unsigned int length;
    void * uh = headerExport(h, &length);

    if (uh == NULL)
	return 1;

    if (magicp == HEADER_MAGIC_YES) {
	nb = Fwrite(rpm_header_magic, sizeof(rpm_header_magic), 1, fd);
	if (nb != sizeof(rpm_header_magic))
	    goto exit;
    }

    nb = Fwrite(uh, sizeof(char), length, fd);

exit:
    free(uh);
    return (nb == length ? 0 : 1);
}