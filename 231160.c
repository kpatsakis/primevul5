ldns_rr_list_cat_clone(const ldns_rr_list *left, const ldns_rr_list *right)
{
	size_t l_rr_count;
	size_t r_rr_count;
	size_t i;
	ldns_rr_list *cat;

	if (left) {
		l_rr_count = ldns_rr_list_rr_count(left);
	} else {
		return ldns_rr_list_clone(right);
	}

	if (right) {
		r_rr_count = ldns_rr_list_rr_count(right);
	} else {
		r_rr_count = 0;
	}

	cat = ldns_rr_list_new();

	if (!cat) {
		return NULL;
	}

	/* left */
	for(i = 0; i < l_rr_count; i++) {
		ldns_rr_list_push_rr(cat,
				ldns_rr_clone(ldns_rr_list_rr(left, i)));
	}
	/* right */
	for(i = 0; i < r_rr_count; i++) {
		ldns_rr_list_push_rr(cat,
				ldns_rr_clone(ldns_rr_list_rr(right, i)));
	}
	return cat;
}