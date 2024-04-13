PHP_FUNCTION(iconv_mime_encode)
{
	zend_string *field_name = NULL;
	zend_string *field_value = NULL;
	zend_string *tmp_str = NULL;
	zval *pref = NULL;
	smart_str retval = {0};
	php_iconv_err_t err;

	const char *in_charset = get_internal_encoding();
	const char *out_charset = in_charset;
	zend_long line_len = 76;
	const char *lfchars = "\r\n";
	php_iconv_enc_scheme_t scheme_id = PHP_ICONV_ENC_SCHEME_BASE64;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "SS|a",
		&field_name, &field_value,
		&pref) == FAILURE) {

		RETURN_FALSE;
	}

	if (pref != NULL) {
		zval *pzval;

		if ((pzval = zend_hash_str_find(Z_ARRVAL_P(pref), "scheme", sizeof("scheme") - 1)) != NULL) {
			if (Z_TYPE_P(pzval) == IS_STRING && Z_STRLEN_P(pzval) > 0) {
				switch (Z_STRVAL_P(pzval)[0]) {
					case 'B': case 'b':
						scheme_id = PHP_ICONV_ENC_SCHEME_BASE64;
						break;

					case 'Q': case 'q':
						scheme_id = PHP_ICONV_ENC_SCHEME_QPRINT;
						break;
				}
			}
		}

		if ((pzval = zend_hash_str_find(Z_ARRVAL_P(pref), "input-charset", sizeof("input-charset") - 1)) != NULL && Z_TYPE_P(pzval) == IS_STRING) {
			if (Z_STRLEN_P(pzval) >= ICONV_CSNMAXLEN) {
				php_error_docref(NULL, E_WARNING, "Charset parameter exceeds the maximum allowed length of %d characters", ICONV_CSNMAXLEN);
				RETURN_FALSE;
			}

			if (Z_STRLEN_P(pzval) > 0) {
				in_charset = Z_STRVAL_P(pzval);
			}
		}


		if ((pzval = zend_hash_str_find(Z_ARRVAL_P(pref), "output-charset", sizeof("output-charset") - 1)) != NULL && Z_TYPE_P(pzval) == IS_STRING) {
			if (Z_STRLEN_P(pzval) >= ICONV_CSNMAXLEN) {
				php_error_docref(NULL, E_WARNING, "Charset parameter exceeds the maximum allowed length of %d characters", ICONV_CSNMAXLEN);
				RETURN_FALSE;
			}

			if (Z_STRLEN_P(pzval) > 0) {
				out_charset = Z_STRVAL_P(pzval);
			}
		}

		if ((pzval = zend_hash_str_find(Z_ARRVAL_P(pref), "line-length", sizeof("line-length") - 1)) != NULL) {
			line_len = zval_get_long(pzval);
		}

		if ((pzval = zend_hash_str_find(Z_ARRVAL_P(pref), "line-break-chars", sizeof("line-break-chars") - 1)) != NULL) {
			if (Z_TYPE_P(pzval) != IS_STRING) {
				tmp_str = zval_get_string(pzval);
				lfchars = ZSTR_VAL(tmp_str);
			} else {
				lfchars = Z_STRVAL_P(pzval);
			}
		}
	}

	err = _php_iconv_mime_encode(&retval, ZSTR_VAL(field_name), ZSTR_LEN(field_name),
		ZSTR_VAL(field_value), ZSTR_LEN(field_value), line_len, lfchars, scheme_id,
		out_charset, in_charset);
	_php_iconv_show_error(err, out_charset, in_charset);

	if (err == PHP_ICONV_ERR_SUCCESS) {
		if (retval.s != NULL) {
			RETVAL_STR(retval.s);
		} else {
			RETVAL_EMPTY_STRING();
		}
	} else {
		smart_str_free(&retval);
		RETVAL_FALSE;
	}

	if (tmp_str) {
		zend_string_release(tmp_str);
	}
}