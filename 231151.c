ldns_rr_new_question_frm_str(ldns_rr **newrr, const char *str,
                             const ldns_rdf *origin, ldns_rdf **prev)
{
	return ldns_rr_new_frm_str_internal(newrr,
	                                    str,
	                                    0,
	                                    origin,
	                                    prev,
	                                    true);
}