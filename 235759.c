static struct efi_time *get_last_timestamp(const char *key, char *last_timestamp)
{
	struct efi_time *timestamp = (struct efi_time*)last_timestamp;

	if (!last_timestamp)
		return NULL;

	if (key_equals(key, "PK"))
		return &timestamp[0];
	else if (key_equals(key, "KEK"))
		return &timestamp[1];
	else if (key_equals(key, "db"))
		return &timestamp[2];
	else if (key_equals(key, "dbx"))
		return &timestamp[3];
	else
		return NULL;
}