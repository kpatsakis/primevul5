int udev_util_encode_string(const char *str, char *str_enc, size_t len)
{
	size_t i, j;

	if (str == NULL || str_enc == NULL || len == 0)
		return -1;

	str_enc[0] = '\0';
	for (i = 0, j = 0; str[i] != '\0'; i++) {
		int seqlen;

		seqlen = utf8_encoded_valid_unichar(&str[i]);
		if (seqlen > 1) {
			memcpy(&str_enc[j], &str[i], seqlen);
			j += seqlen;
			i += (seqlen-1);
		} else if (str[i] == '\\' || !is_whitelisted(str[i], NULL)) {
			sprintf(&str_enc[j], "\\x%02x", (unsigned char) str[i]);
			j += 4;
		} else {
			str_enc[j] = str[i];
			j++;
		}
		if (j+3 >= len)
			goto err;
	}
	str_enc[j] = '\0';
	return 0;
err:
	return -1;
}