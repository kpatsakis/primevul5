int check_timestamp(const char *key, const struct efi_time *timestamp,
		    char *last_timestamp)
{
	struct efi_time *prev;
	uint64_t new;
	uint64_t last;

	prev = get_last_timestamp(key, last_timestamp);
	if (prev == NULL)
		return OPAL_INTERNAL_ERROR;

	prlog(PR_DEBUG, "timestamp year is %d month %d day %d\n",
			le16_to_cpu(timestamp->year), timestamp->month,
			timestamp->day);
	prlog(PR_DEBUG, "prev year is %d month %d day %d\n",
			le16_to_cpu(prev->year), prev->month, prev->day);

	new = unpack_timestamp(timestamp);
	last = unpack_timestamp(prev);

	if (new > last)
		return OPAL_SUCCESS;

	return OPAL_PERMISSION;
}