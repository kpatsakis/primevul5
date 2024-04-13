Header headerRead(FD_t fd, int magicp)
{
    Header h = NULL;
    struct hdrblob_s blob;
    char *buf = NULL;

    if (hdrblobRead(fd, magicp, 0, 0, &blob, &buf) == RPMRC_OK)
	hdrblobImport(&blob, 0, &h, &buf);

    free(buf);
    return h;
}