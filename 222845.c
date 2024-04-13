static void load_creator(FILE *fp, pdf_t *pdf)
{
    int    i, buf_idx;
    char   c, *buf, obj_id_buf[32] = {0};
    long   start;
    size_t sz;

    start = ftell(fp);

    /* For each PDF version */
    for (i=0; i<pdf->n_xrefs; ++i)
    {
        if (!pdf->xrefs[i].version)
          continue;

        /* Find trailer */
        fseek(fp, pdf->xrefs[i].start, SEEK_SET);
        while (SAFE_F(fp, (fgetc(fp) != 't')))
            ; /* Iterate to "trailer" */

        /* Look for "<< ....... /Info ......" */
        c = '\0';
        while (SAFE_F(fp, ((c = fgetc(fp)) != '>')))
          if (SAFE_F(fp, ((c == '/') &&
                          (fgetc(fp) == 'I') && ((fgetc(fp) == 'n')))))
            break;

        /* Could not find /Info in trailer */
        END_OF_TRAILER(c, start, fp);

        while (SAFE_F(fp, (!isspace(c = fgetc(fp)) && (c != '>'))))
            ; /* Iterate to first white space /Info<space><data> */

        /* No space between /Info and its data */
        END_OF_TRAILER(c, start, fp);

        while (SAFE_F(fp, (isspace(c = fgetc(fp)) && (c != '>'))))
            ; /* Iterate right on top of first non-whitespace /Info data */

        /* No data for /Info */
        END_OF_TRAILER(c, start, fp);

        /* Get obj id as number */
        buf_idx = 0;
        obj_id_buf[buf_idx++] = c;
        while ((buf_idx < (sizeof(obj_id_buf) - 1)) &&
               SAFE_F(fp, (!isspace(c = fgetc(fp)) && (c != '>'))))
          obj_id_buf[buf_idx++] = c;

        END_OF_TRAILER(c, start, fp);
     
        /* Get the object for the creator data.  If linear, try both xrefs */ 
        buf = get_object(fp, atoll(obj_id_buf), &pdf->xrefs[i], &sz, NULL);
        if (!buf && pdf->xrefs[i].is_linear && (i+1 < pdf->n_xrefs))
          buf = get_object(fp, atoll(obj_id_buf), &pdf->xrefs[i+1], &sz, NULL);

        load_creator_from_buf(fp, &pdf->xrefs[i], buf, sz);
        free(buf);
    }

    fseek(fp, start, SEEK_SET);
}