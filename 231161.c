ldns_rr_list_subtype_by_rdf(const ldns_rr_list *l, const ldns_rdf *r, size_t pos)
{
	size_t i;
	ldns_rr_list *subtyped;
	ldns_rdf *list_rdf;

	subtyped = ldns_rr_list_new();

	for(i = 0; i < ldns_rr_list_rr_count(l); i++) {
		list_rdf = ldns_rr_rdf(
			ldns_rr_list_rr(l, i),
			pos);
		if (!list_rdf) {
			/* pos is too large or any other error */
			ldns_rr_list_deep_free(subtyped);
			return NULL;
		}

		if (ldns_rdf_compare(list_rdf, r) == 0) {
			/* a match */
			ldns_rr_list_push_rr(subtyped,
					ldns_rr_clone(ldns_rr_list_rr(l, i)));
		}
	}

	if (ldns_rr_list_rr_count(subtyped) > 0) {
		return subtyped;
	} else {
		ldns_rr_list_free(subtyped);
		return NULL;
	}
}