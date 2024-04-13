static const char *get_type(FILE *fp, int obj_id, const xref_t *xref)
{
    int          is_stream;
    char        *c, *obj, *endobj;
    static char  buf[32];
    long         start;

    start = ftell(fp);

    if (!(obj = get_object(fp, obj_id, xref, NULL, &is_stream)) || 
        is_stream                                               ||
        !(endobj = strstr(obj, "endobj")))
    {
        free(obj);
        fseek(fp, start, SEEK_SET);

        if (is_stream)
          return "Stream";
        else
          return "Unknown";
    }

    /* Get the Type value (avoiding font names like Type1) */
    c = obj;
    while ((c = strstr(c, "/Type")) && (c < endobj))
      if (isdigit(*(c + strlen("/Type"))))
      {
          ++c;
          continue;
      }
      else
        break;

    if (!c || (c && (c > endobj)))
    {
        free(obj);
        fseek(fp, start, SEEK_SET);
        return "Unknown";
    }

    /* Skip to first blank/whitespace */
    c += strlen("/Type");
    while (isspace(*c) || *c == '/')
      ++c;

    /* 'c' should be pointing to the type name.  Find the end of the name. */
    size_t n_chars = 0;
    const char *name_itr = c;
    while ((name_itr < endobj) &&
           !(isspace(*name_itr) || *name_itr == '/' || *name_itr == '>')) {
        ++name_itr;
        ++n_chars;
    }
    if (n_chars >= sizeof(buf))
    {
        free(obj);
        fseek(fp, start, SEEK_SET);
        return "Unknown";
    }

    /* Return the value by storing it in static mem. */
    memcpy(buf, c, n_chars);
    buf[n_chars] = '\0';
    free(obj);
    fseek(fp, start, SEEK_SET);
    return buf;
}