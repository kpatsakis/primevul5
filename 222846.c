static char *get_object_from_here(FILE *fp, size_t *size, int *is_stream)
{
    long         start;
    char         buf[256];
    int          obj_id;
    xref_t       xref;
    xref_entry_t entry;

    start = ftell(fp);

    /* Object ID */
    memset(buf, 0, 256);
    SAFE_E(fread(buf, 1, 255, fp), 255, "Failed to load object ID.\n");
    if (!(obj_id = atoi(buf)))
    {
        fseek(fp, start, SEEK_SET);
        return NULL;
    }
    
    /* Create xref entry to pass to the get_object routine */
    memset(&entry, 0, sizeof(xref_entry_t));
    entry.obj_id = obj_id;
    entry.offset = start;

    /* Xref and single entry for the object we want data from */
    memset(&xref, 0, sizeof(xref_t));
    xref.n_entries = 1;
    xref.entries = &entry;

    fseek(fp, start, SEEK_SET);
    return get_object(fp, obj_id, &xref, size, is_stream);
}