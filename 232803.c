_asnstr_append_name(char *name, size_t name_size, const char *part1,
		    const char *part2)
{
	if (part1[0] != 0) {
		_gnutls_str_cpy(name, name_size, part1);
		_gnutls_str_cat(name, name_size, part2);
	} else
		_gnutls_str_cpy(name, name_size,
				part2 + 1 /* remove initial dot */ );
}