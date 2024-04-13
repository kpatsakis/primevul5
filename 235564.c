static off_t matchlen(unsigned char *old, off_t oldsize,
                      unsigned char *new, off_t newsize)
{
    off_t i;

    for (i = 0; (i < oldsize) && (i < newsize); i++)
        if (old[i] != new[i])
            break;
    return i;
}