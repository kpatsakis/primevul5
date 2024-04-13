void * headerExport(Header h, unsigned int *bsize)
{
    void *blob = NULL;

    if (h) {
	blob = doExport(h->index, h->indexUsed, h->flags, bsize);
    }

    return blob;
}