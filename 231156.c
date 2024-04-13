ldns_rr_new_frm_str(ldns_rr **newrr, const char *str,
                    uint32_t default_ttl, const ldns_rdf *origin,
				    ldns_rdf **prev)
{
	return ldns_rr_new_frm_str_internal(newrr,
	                                    str,
	                                    default_ttl,
	                                    origin,
	                                    prev,
	                                    false);
}