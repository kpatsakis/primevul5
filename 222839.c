static void load_creator_from_old_format(
    FILE       *fp,
    xref_t     *xref,
    const char *buf,
    size_t      buf_size)
{
    int            i, n_eles, length, is_escaped, obj_id;
    char          *c, *ascii, *start, *s, *saved_buf_search, *obj;
    size_t         obj_size;
    pdf_creator_t *info;

    info = new_creator(&n_eles);

    /* Mark the end of buf, so that we do not crawl past it */
    if (buf_size < 1) return;
    const char *buf_end = buf + buf_size - 1;

    /* Treat 'end' as either the end of 'buf' or the end of 'obj'.  Obj is if
     * the creator element (e.g., ModDate, Producer, etc) is an object and not
     * part of 'buf'.
     */
    const char *end = buf_end;

    for (i=0; i<n_eles; ++i)
    {
        if (!(c = strstr(buf, info[i].key)))
          continue;

        /* Find the value (skipping whitespace) */
        c += strlen(info[i].key);
        while (isspace(*c))
          ++c;
        if (c >= buf_end) {
          FAIL("Failed to locate space, likely a corrupt PDF.");
        }

        /* If looking at the start of a pdf token, we have gone too far */
        if (*c == '/')
          continue;

        /* If the value is a number and not a '(' then the data is located in
         * an object we need to fetch, and not inline
         */
        obj = saved_buf_search = NULL;
        obj_size = 0;
        end = buf_end; /* Init to be the buffer, this might not be an obj. */
        if (isdigit(*c))
        {
            obj_id = atoi(c);
            saved_buf_search = c;
            s = saved_buf_search;

            obj = get_object(fp, obj_id, xref, &obj_size, NULL);
            end = obj + obj_size;
            c = obj;

            /* Iterate to '(' */
            while (c && (*c != '(') && (c < end))
              ++c;
            if (c >= end)  {
              FAIL("Failed to locate a '(' character. "
                  "This might be a corrupt PDF.\n");
            }

            /* Advance the search to the next token */
            while (s && (*s == '/') && (s < buf_end))
              ++s;
            if (s >= buf_end)  {
              FAIL("Failed to locate a '/' character. "
                  "This might be a corrupt PDF.\n");
            }
            saved_buf_search = s;
        }
          
        /* Find the end of the value */
        start = c;
        length = is_escaped = 0;
        while (c && ((*c != '\r') && (*c != '\n') && (*c != '<')))
        {
            /* Bail out if we see an un-escaped ')' closing character */
            if (!is_escaped && (*c == ')'))
              break;
            else if (*c == '\\')
              is_escaped = 1;
            else
              is_escaped = 0;
            ++c;
            ++length;
            if (c > end) {
              FAIL("Failed to locate the end of a value. "
                   "This might be a corrupt PDF.\n");
            }
        }

        if (length == 0)
          continue;

        /* Add 1 to length so it gets the closing ')' when we copy */
        if (length)
          length += 1;
        length = (length > KV_MAX_VALUE_LENGTH) ? KV_MAX_VALUE_LENGTH : length;
        strncpy(info[i].value, start, length);
        info[i].value[KV_MAX_VALUE_LENGTH - 1] = '\0';

        /* Restore where we were searching from */
        if (saved_buf_search)
        {
            /* Release memory from get_object() called earlier */
            free(obj);
            c = saved_buf_search;
        }
    } /* For all creation information tags */

    /* Go through the values and convert if encoded */
    for (i = 0; i < n_eles; ++i) {
      const size_t val_str_len = strnlen(info[i].value, KV_MAX_VALUE_LENGTH);
      if ((ascii = decode_text_string(info[i].value, val_str_len))) {
        strncpy(info[i].value, ascii, val_str_len);
        free(ascii);
      }
    }

    xref->creator = info;
    xref->n_creator_entries = n_eles;
}