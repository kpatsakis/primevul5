static void resolve_linearized_pdf(pdf_t *pdf)
{
    int    i;
    xref_t buf;

    if (pdf->n_xrefs < 2)
      return;

    if (!pdf->xrefs[0].is_linear)
      return;

    /* Swap Linear with Version 1 */
    buf = pdf->xrefs[0];
    pdf->xrefs[0] = pdf->xrefs[1];
    pdf->xrefs[1] = buf;

    /* Resolve is_linear flag and version */
    pdf->xrefs[0].is_linear = 1;
    pdf->xrefs[0].version = 1;
    pdf->xrefs[1].is_linear = 0;
    pdf->xrefs[1].version = 1;

    /* Adjust the other version values now */
    for (i=2; i<pdf->n_xrefs; ++i)
      --pdf->xrefs[i].version;
}