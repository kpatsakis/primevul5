static int oidc_delete_oldest_state_cookies(request_rec *r,
		int number_of_valid_state_cookies, int max_number_of_state_cookies,
		oidc_state_cookies_t *first) {
	oidc_state_cookies_t *cur = NULL, *prev = NULL, *prev_oldest = NULL,
			*oldest = NULL;
	while (number_of_valid_state_cookies >= max_number_of_state_cookies) {
		oldest = first;
		prev_oldest = NULL;
		prev = first;
		cur = first->next;
		while (cur) {
			if ((cur->timestamp < oldest->timestamp)) {
				oldest = cur;
				prev_oldest = prev;
			}
			prev = cur;
			cur = cur->next;
		}
		oidc_warn(r,
				"deleting oldest state cookie: %s (time until expiry %" APR_TIME_T_FMT " seconds)",
				oldest->name, apr_time_sec(oldest->timestamp - apr_time_now()));
		oidc_util_set_cookie(r, oldest->name, "", 0, NULL);
		if (prev_oldest)
			prev_oldest->next = oldest->next;
		else
			first = first->next;
		number_of_valid_state_cookies--;
	}
	return number_of_valid_state_cookies;
}