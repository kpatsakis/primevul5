verbose_success(pam_handle_t *pamh, long diff)
{
	pam_info(pamh, _("Access granted (last access was %ld seconds ago)."), diff);
}