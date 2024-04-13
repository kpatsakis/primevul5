HeaderIterator headerFreeIterator(HeaderIterator hi)
{
    if (hi != NULL) {
	hi->h = headerFree(hi->h);
	hi = _free(hi);
    }
    return NULL;
}