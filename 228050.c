static int __buffer_append_option(struct ul_buffer *buf,
			const char *name, size_t namesz,
			const char *val, size_t valsz)
{
	int rc = 0;

	if (!ul_buffer_is_empty(buf))
		rc = ul_buffer_append_data(buf, ",", 1);
	if (!rc)
		rc = ul_buffer_append_data(buf, name, namesz);
	if (val && !rc) {
		/* we need to append '=' is value is empty string, see
		 * 727c689908c5e68c92aa1dd65e0d3bdb6d91c1e5 */
		rc = ul_buffer_append_data(buf, "=", 1);
		if (!rc && valsz)
			rc = ul_buffer_append_data(buf, val, valsz);
	}
	return rc;
}