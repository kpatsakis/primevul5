static void load_xref_entries(FILE *fp, xref_t *xref)
{
    if (xref->is_stream)
      load_xref_from_stream(fp, xref);
    else
      load_xref_from_plaintext(fp, xref);
}