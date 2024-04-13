static void load_creator_from_buf(
    FILE       *fp,
    xref_t     *xref,
    const char *buf,
    size_t      buf_size)
{
    int   is_xml;
    char *c;

    if (!buf)
      return;

    /* Check to see if this is xml or old-school */
    if ((c = strstr(buf, "/Type")))
      while (*c && !isspace(*c))
        ++c;

    /* Probably "Metadata" */
    is_xml = 0;
    if (c && (*c == 'M'))
      is_xml = 1;

    /* Is the buffer XML(PDF 1.4+) or old format? */
    if (is_xml)
      load_creator_from_xml(xref, buf);
    else
      load_creator_from_old_format(fp, xref, buf, buf_size);
}