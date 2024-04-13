ldns_rr_list_cat(ldns_rr_list *left, const ldns_rr_list *right)
{
	size_t r_rr_count;
	size_t i;

	if (!left) {
		return false;
	}

	if (right) {
		r_rr_count = ldns_rr_list_rr_count(right);
	} else {
		r_rr_count = 0;
	}

	/* push right to left */
	for(i = 0; i < r_rr_count; i++) {
		ldns_rr_list_push_rr(left, ldns_rr_list_rr(right, i));
	}
	return true;
}