void pdf_get_version(FILE *fp, pdf_t *pdf)
{
    char *header = get_header(fp);

    /* Locate version string start and make sure we dont go past header
     * The format is %PDF-M.m, where 'M' is the major number and 'm' minor.
     */
    const char *c;
    if ((c = strstr(header, "%PDF-")) && 
        ((c + 6)[0] == '.') && // Separator
        isdigit((c + 5)[0]) && // Major number
        isdigit((c + 7)[0]))   // Minor number
    {
        pdf->pdf_major_version = atoi(c + strlen("%PDF-"));
        pdf->pdf_minor_version = atoi(c + strlen("%PDF-M."));
    }

    free(header);
}