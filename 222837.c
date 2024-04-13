void pdf_zero_object(
    FILE        *fp,
    const pdf_t *pdf,
    int          xref_idx,
    int          entry_idx)
{
    int           i;
    char         *obj;
    size_t        obj_sz;
    xref_entry_t *entry;

    entry = &pdf->xrefs[xref_idx].entries[entry_idx];
    fseek(fp, entry->offset, SEEK_SET);

    /* Get object and size */
    obj = get_object(fp, entry->obj_id, &pdf->xrefs[xref_idx], NULL, NULL);
    i = obj_sz = 0;
    while (strncmp((++i)+obj, "endobj", 6))
      ++obj_sz;

    if (obj_sz)
      obj_sz += strlen("endobj") + 1;

    /* Zero object */
    for (i=0; i<obj_sz; i++)
      fputc('0', fp);

    printf("Zeroed object %d\n", entry->obj_id);
    free(obj);
}