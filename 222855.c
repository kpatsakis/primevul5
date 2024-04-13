static int is_valid_xref(FILE *fp, pdf_t *pdf, xref_t *xref)
{
    int   is_valid;
    long  start;
    char *c, buf[16];
    
    memset(buf, 0, sizeof(buf));
    is_valid = 0;
    start = ftell(fp);
    fseek(fp, xref->start, SEEK_SET);

    if (fgets(buf, 16, fp) == NULL) {
      ERR("Failed to load xref string.");
      exit(EXIT_FAILURE);
    }

    if (strncmp(buf, "xref", strlen("xref")) == 0)
      is_valid = 1;
    else
    {  
        /* PDFv1.5+ allows for xref data to be stored in streams vs plaintext */
        fseek(fp, xref->start, SEEK_SET);
        c = get_object_from_here(fp, NULL, &xref->is_stream);

        if (c && xref->is_stream)
        {
            pdf->has_xref_streams = 1;
            is_valid = 1;
        }
        free(c);
    }

    fseek(fp, start, SEEK_SET);
    return is_valid;
}