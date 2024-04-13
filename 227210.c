const char *bcf_hdr_get_version(const bcf_hdr_t *hdr)
{
    bcf_hrec_t *hrec = bcf_hdr_get_hrec(hdr, BCF_HL_GEN, "fileformat", NULL, NULL);
    if ( !hrec )
    {
        hts_log_warning("No version string found, assuming VCFv4.2");
        return "VCFv4.2";
    }
    return hrec->value;
}