int update_timestamp(const char *key, const struct efi_time *timestamp, char *last_timestamp)
{
	struct efi_time *prev;

	prev = get_last_timestamp(key, last_timestamp);
	if (prev == NULL)
		return OPAL_INTERNAL_ERROR;

	/* Update with new timestamp */
	memcpy(prev, timestamp, sizeof(struct efi_time));

	prlog(PR_DEBUG, "updated prev year is %d month %d day %d\n",
			le16_to_cpu(prev->year), prev->month, prev->day);

	return OPAL_SUCCESS;
}