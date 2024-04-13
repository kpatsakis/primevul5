STATIC GC_descr GC_double_descr(GC_descr descriptor, word nwords)
{
    if ((descriptor & GC_DS_TAGS) == GC_DS_LENGTH) {
        descriptor = GC_bm_table[BYTES_TO_WORDS((word)descriptor)];
    };
    descriptor |= (descriptor & ~GC_DS_TAGS) >> nwords;
    return(descriptor);
}