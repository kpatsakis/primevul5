ldns_rr_label_count(const ldns_rr *rr)
{
	if (!rr) {
		return 0;
	}
	return ldns_dname_label_count(
			ldns_rr_owner(rr));
}