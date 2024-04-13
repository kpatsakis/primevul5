static void load_xref_from_stream(FILE *fp, xref_t *xref)
{
    long    start;
    int     is_stream;
    char   *stream;
    size_t  size;

    start = ftell(fp);
    fseek(fp, xref->start, SEEK_SET);

    stream = NULL;
    stream = get_object_from_here(fp, &size, &is_stream);
    fseek(fp, start, SEEK_SET);

    /* TODO: decode and analyize stream */
    free(stream);
    return;
}