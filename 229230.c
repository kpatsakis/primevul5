enumerate_providers_fn (const char * const name,
                        const char * const desc,
                        const char * const file,
                        void * ud) /* {{{ */
{
	zval *zdesc = (zval *) ud;
	zval *tmp_array;

	MAKE_STD_ZVAL(tmp_array);
	array_init(tmp_array);

	add_assoc_string(tmp_array, "name", (char *)name, 1);
	add_assoc_string(tmp_array, "desc", (char *)desc, 1);
	add_assoc_string(tmp_array, "file", (char *)file, 1);

	if (Z_TYPE_P(zdesc)!=IS_ARRAY) {
		array_init(zdesc);
	}

	add_next_index_zval(zdesc, tmp_array);
}