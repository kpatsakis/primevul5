ldns_rr_list_contains_rr(const ldns_rr_list *rr_list, const ldns_rr *rr)
{
	size_t i;

	if (!rr_list || !rr || ldns_rr_list_rr_count(rr_list) == 0) {
		return false;
	}

	for (i = 0; i < ldns_rr_list_rr_count(rr_list); i++) {
		if (rr == ldns_rr_list_rr(rr_list, i)) {
			return true;
		} else if (ldns_rr_compare(rr, ldns_rr_list_rr(rr_list, i)) == 0) {
			return true;
		}
	}
	return false;
}