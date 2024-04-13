int mnt_optstr_get_uid(const char *optstr, const char *name, uid_t *uid)
{
	char *value = NULL;
	size_t valsz = 0;
	char buf[sizeof(stringify_value(UINT64_MAX))];
	int rc;
	uint64_t num;

	assert(optstr);
	assert(name);
	assert(uid);

	rc = mnt_optstr_get_option(optstr, name, &value, &valsz);
	if (rc != 0)
		goto fail;

	if (valsz > sizeof(buf) - 1) {
		rc = -ERANGE;
		goto fail;
	}
	mem2strcpy(buf, value, valsz, sizeof(buf));

	rc = ul_strtou64(buf, &num, 10);
	if (rc != 0)
		goto fail;
	if (num > ULONG_MAX || (uid_t) num != num) {
		rc = -ERANGE;
		goto fail;
	}
	*uid = (uid_t) num;

	return 0;
fail:
	DBG(UTILS, ul_debug("failed to convert '%s'= to number [rc=%d]", name, rc));
	return rc;
}