ldns_is_rrset(const ldns_rr_list *rr_list)
{
	ldns_rr_type t;
	ldns_rr_class c;
	ldns_rdf *o;
	ldns_rr *tmp;
	size_t i;

	if (!rr_list || ldns_rr_list_rr_count(rr_list) == 0) {
		return false;
	}

	tmp = ldns_rr_list_rr(rr_list, 0);

	t = ldns_rr_get_type(tmp);
	c = ldns_rr_get_class(tmp);
	o = ldns_rr_owner(tmp);

	/* compare these with the rest of the rr_list, start with 1 */
	for (i = 1; i < ldns_rr_list_rr_count(rr_list); i++) {
		tmp = ldns_rr_list_rr(rr_list, i);
		if (t != ldns_rr_get_type(tmp)) {
			return false;
		}
		if (c != ldns_rr_get_class(tmp)) {
			return false;
		}
		if (ldns_rdf_compare(o, ldns_rr_owner(tmp)) != 0) {
			return false;
		}
	}
	return true;
}