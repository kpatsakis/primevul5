static void php_enchant_list_dicts_fn( const char * const lang_tag,
	   	const char * const provider_name, const char * const provider_desc,
		const char * const provider_file, void * ud) /* {{{ */
{
	zval *zdesc = (zval *) ud;
	zval *tmp_array;

	MAKE_STD_ZVAL(tmp_array);
	array_init(tmp_array);
	add_assoc_string(tmp_array, "lang_tag", (char *)lang_tag, 1);
	add_assoc_string(tmp_array, "provider_name", (char *)provider_name, 1);
	add_assoc_string(tmp_array, "provider_desc", (char *)provider_desc, 1);
	add_assoc_string(tmp_array, "provider_file", (char *)provider_file, 1);

	if (Z_TYPE_P(zdesc) != IS_ARRAY) {
		array_init(zdesc);
	}
	add_next_index_zval(zdesc, tmp_array);

}