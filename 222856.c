static void load_xref_from_plaintext(FILE *fp, xref_t *xref)
{
    int  i, obj_id, added_entries;
    char c, buf[32] = {0};
    long start, pos;
    size_t buf_idx;

    start = ftell(fp);

    /* Get number of entries */
    pos = xref->end;
    fseek(fp, pos, SEEK_SET);
    while (ftell(fp) != 0)
      if (SAFE_F(fp, (fgetc(fp) == '/' && fgetc(fp) == 'S')))
        break;
      else
        SAFE_E(fseek(fp, --pos, SEEK_SET), 0, "Failed seek to xref /Size.\n");

    SAFE_E(fread(buf, 1, 21, fp), 21, "Failed to load entry Size string.\n");
    xref->n_entries = atoi(buf + strlen("ize "));
    xref->entries = safe_calloc(xref->n_entries * sizeof(struct _xref_entry));

    /* Load entry data */
    obj_id = 0;
    fseek(fp, xref->start + strlen("xref"), SEEK_SET);
    added_entries = 0;
    for (i=0; i<xref->n_entries; i++)
    {
        /* Advance past newlines. */
        c = fgetc(fp);
        while (c == '\n' || c == '\r')
          c = fgetc(fp);

        if (ferror(fp) || feof(fp))
          break;

        /* Collect data up until the following newline. */
        buf_idx = 0;
        while (c != '\n' && c != '\r' && !feof(fp) &&
               !ferror(fp) && buf_idx < sizeof(buf))
        {
            buf[buf_idx++] = c;
            c = fgetc(fp);
        }
        if (buf_idx >= sizeof(buf)) {
            FAIL("Failed to locate newline character. "
                 "This might be a corrupt PDF.\n");
        }
        buf[buf_idx] = '\0';

        /* Went to far and hit start of trailer */
        if (strchr(buf, 't'))
          break;

        /* Entry or object id */
        if (strlen(buf) > 17)
        {
            const char *token = NULL;
            xref->entries[i].obj_id = obj_id++;
            token = strtok(buf, " ");
            if (!token) {
              FAIL("Failed to parse xref entry. "
                   "This might be a corrupt PDF.\n");
            }
            xref->entries[i].offset = atol(token);
            token = strtok(NULL, " ");
            if (!token) {
              FAIL("Failed to parse xref entry. "
                   "This might be a corrupt PDF.\n");
            }
            xref->entries[i].gen_num = atoi(token);
            xref->entries[i].f_or_n = buf[17];
            ++added_entries;
        }
        else
        {
            obj_id = atoi(buf);
            --i;
        }
    }

    xref->n_entries = added_entries;
    fseek(fp, start, SEEK_SET);
}