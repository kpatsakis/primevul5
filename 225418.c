PHP_FUNCTION(iconv_mime_decode_headers)
{
	zend_string *encoded_str;
	char *charset = get_internal_encoding();
	size_t charset_len = 0;
	zend_long mode = 0;
	char *enc_str_tmp;
	size_t enc_str_len_tmp;

	php_iconv_err_t err = PHP_ICONV_ERR_SUCCESS;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|ls",
		&encoded_str, &mode, &charset, &charset_len) == FAILURE) {

		RETURN_FALSE;
	}

	if (charset_len >= ICONV_CSNMAXLEN) {
		php_error_docref(NULL, E_WARNING, "Charset parameter exceeds the maximum allowed length of %d characters", ICONV_CSNMAXLEN);
		RETURN_FALSE;
	}

	array_init(return_value);

	enc_str_tmp = ZSTR_VAL(encoded_str);
	enc_str_len_tmp = ZSTR_LEN(encoded_str);
	while (enc_str_len_tmp > 0) {
		smart_str decoded_header = {0};
		char *header_name = NULL;
		size_t header_name_len = 0;
		char *header_value = NULL;
		size_t header_value_len = 0;
		char *p, *limit;
		const char *next_pos;

		if (PHP_ICONV_ERR_SUCCESS != (err = _php_iconv_mime_decode(&decoded_header, enc_str_tmp, enc_str_len_tmp, charset, &next_pos, (int)mode))) {
			smart_str_free(&decoded_header);
			break;
		}

		if (decoded_header.s == NULL) {
			break;
		}

		limit = ZSTR_VAL(decoded_header.s) + ZSTR_LEN(decoded_header.s);
		for (p = ZSTR_VAL(decoded_header.s); p < limit; p++) {
			if (*p == ':') {
				*p = '\0';
				header_name = ZSTR_VAL(decoded_header.s);
				header_name_len = p - ZSTR_VAL(decoded_header.s);

				while (++p < limit) {
					if (*p != ' ' && *p != '\t') {
						break;
					}
				}

				header_value = p;
				header_value_len = limit - p;

				break;
			}
		}

		if (header_name != NULL) {
			zval *elem;

			if ((elem = zend_hash_str_find(Z_ARRVAL_P(return_value), header_name, header_name_len)) != NULL) {
				if (Z_TYPE_P(elem) != IS_ARRAY) {
					zval new_elem;

					array_init(&new_elem);
					Z_ADDREF_P(elem);
					add_next_index_zval(&new_elem, elem);

					elem = zend_hash_str_update(Z_ARRVAL_P(return_value), header_name, header_name_len, &new_elem);
				}
				add_next_index_stringl(elem, header_value, header_value_len);
			} else {
				add_assoc_stringl_ex(return_value, header_name, header_name_len, header_value, header_value_len);
			}
		}
		enc_str_len_tmp -= next_pos - enc_str_tmp;
		enc_str_tmp = (char *)next_pos;

		smart_str_free(&decoded_header);
	}

	if (err != PHP_ICONV_ERR_SUCCESS) {
		_php_iconv_show_error(err, charset, "???");
		zval_dtor(return_value);
		RETVAL_FALSE;
	}
}