static void msgfmt_do_parse(MessageFormatter_object *mfo, char *source, size_t src_len, zval *return_value)
{
	zval *fargs;
	int count = 0;
	int i;
	UChar *usource = NULL;
	int usrc_len = 0;

	intl_convert_utf8_to_utf16(&usource, &usrc_len, source, src_len, &INTL_DATA_ERROR_CODE(mfo));
	INTL_METHOD_CHECK_STATUS(mfo, "Converting parse string failed");

	umsg_parse_helper(MSG_FORMAT_OBJECT(mfo), &count, &fargs, usource, usrc_len, &INTL_DATA_ERROR_CODE(mfo));
	if (usource) {
		efree(usource);
	}
	INTL_METHOD_CHECK_STATUS(mfo, "Parsing failed");

	array_init(return_value);
	for(i=0;i<count;i++) {
		add_next_index_zval(return_value, &fargs[i]);
	}
	efree(fargs);
}