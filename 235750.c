static void get_key_authority(const char *ret[3], const char *key)
{
	int i = 0;

	if (key_equals(key, "PK")) {
		ret[i++] = "PK";
	} else if (key_equals(key, "KEK")) {
		ret[i++] = "PK";
	} else if (key_equals(key, "db") || key_equals(key, "dbx")) {
		ret[i++] = "KEK";
		ret[i++] = "PK";
	}

	ret[i] = NULL;
}