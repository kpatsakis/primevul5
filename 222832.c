int pdf_display_creator(const pdf_t *pdf, int xref_idx)
{
    int i;

    if (!pdf->xrefs[xref_idx].creator)
      return 0;

    for (i=0; i<pdf->xrefs[xref_idx].n_creator_entries; ++i)
      printf("%s: %s\n",
             pdf->xrefs[xref_idx].creator[i].key,
             pdf->xrefs[xref_idx].creator[i].value);

    return (i > 0);
}