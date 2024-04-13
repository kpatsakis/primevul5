ldns_rr_list2canonical(const ldns_rr_list *rr_list)
{
	size_t i;
	for (i = 0; i < ldns_rr_list_rr_count(rr_list); i++) {
		ldns_rr2canonical(ldns_rr_list_rr(rr_list, i));
	}
}