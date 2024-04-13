int bcf_hdr_parse(bcf_hdr_t *hdr, char *htxt)
{
    int len, done = 0;
    char *p = htxt;

    // Check sanity: "fileformat" string must come as first
    bcf_hrec_t *hrec = bcf_hdr_parse_line(hdr,p,&len);
    if ( !hrec || !hrec->key || strcasecmp(hrec->key,"fileformat") )
        hts_log_warning("The first line should be ##fileformat; is the VCF/BCF header broken?");
    if (bcf_hdr_add_hrec(hdr, hrec) < 0) {
        bcf_hrec_destroy(hrec);
        return -1;
    }

    // The filter PASS must appear first in the dictionary
    hrec = bcf_hdr_parse_line(hdr,"##FILTER=<ID=PASS,Description=\"All filters passed\">",&len);
    if (bcf_hdr_add_hrec(hdr, hrec) < 0) {
        bcf_hrec_destroy(hrec);
        return -1;
    }

    // Parse the whole header
    do {
        while (NULL != (hrec = bcf_hdr_parse_line(hdr, p, &len))) {
            if (bcf_hdr_add_hrec(hdr, hrec) < 0) {
                bcf_hrec_destroy(hrec);
                return -1;
            }
            p += len;
        }

        // Next should be the sample line.  If not, it was a malformed
        // header, in which case print a warning and skip (many VCF
        // operations do not really care about a few malformed lines).
        // In the future we may want to add a strict mode that errors in
        // this case.
        if ( strncmp("#CHROM\tPOS",p,10) != 0 ) {
            char *eol = strchr(p, '\n');
            if (*p != '\0') {
                hts_log_warning("Could not parse header line: %.*s",
                    eol ? (int)(eol - p) : INT_MAX, p);
            }
            if (eol) {
                p = eol + 1; // Try from the next line.
            } else {
                done = -1; // No more lines left, give up.
            }
        } else {
            done = 1; // Sample line found
        }
    } while (!done);

    if (done < 0) {
        // No sample line is fatal.
        hts_log_error("Could not parse the header, sample line not found");
        return -1;
    }

    if (bcf_hdr_parse_sample_line(hdr,p) < 0)
        return -1;
    if (bcf_hdr_sync(hdr) < 0)
        return -1;
    bcf_hdr_check_sanity(hdr);
    return 0;
}