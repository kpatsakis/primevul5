Header headerImport(void * blob, unsigned int bsize, headerImportFlags flags)
{
    Header h = NULL;
    struct hdrblob_s hblob;
    char *buf = NULL;
    void * b = blob;

    if (flags & HEADERIMPORT_COPY) {
	if (bsize == 0 && hdrblobInit(b, 0, 0, 0, &hblob, &buf) == RPMRC_OK)
	    bsize = hblob.pvlen;
	if (bsize == 0)
	    goto exit;
	b = memcpy(xmalloc(bsize), b, bsize);
    }

    /* Sanity checks on header intro. */
    if (hdrblobInit(b, bsize, 0, 0, &hblob, &buf) == RPMRC_OK)
	hdrblobImport(&hblob, (flags & HEADERIMPORT_FAST), &h, &buf);

exit:
    if (h == NULL && b != blob)
	free(b);
    free(buf);

    return h;
}