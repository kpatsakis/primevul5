static int _build_trace_args(zval **arg TSRMLS_DC, int num_args, va_list args, zend_hash_key *hash_key) /* {{{ */
{
	char **str;
	int *len;

	str = va_arg(args, char**);
	len = va_arg(args, int*);

	/* the trivial way would be to do:
	 * convert_to_string_ex(arg);
	 * append it and kill the now tmp arg.
	 * but that could cause some E_NOTICE and also damn long lines.
	 */

	switch (Z_TYPE_PP(arg)) {
		case IS_NULL:
			TRACE_APPEND_STR("NULL, ");
			break;
		case IS_STRING: {
			int l_added;
			TRACE_APPEND_CHR('\'');
			if (Z_STRLEN_PP(arg) > 15) {
				TRACE_APPEND_STRL(Z_STRVAL_PP(arg), 15);
				TRACE_APPEND_STR("...', ");
				l_added = 15 + 6 + 1; /* +1 because of while (--l_added) */
			} else {
				l_added = Z_STRLEN_PP(arg);
				TRACE_APPEND_STRL(Z_STRVAL_PP(arg), l_added);
				TRACE_APPEND_STR("', ");
				l_added += 3 + 1;
			}
			while (--l_added) {
				unsigned char chr = (*str)[*len - l_added];
				if (chr < 32 || chr == '\\' || chr > 126) {
					(*str)[*len - l_added] = '\\';

					switch (chr) {
						case '\n':
							TRACE_ARG_APPEND(1);
							(*str)[++(*len) - l_added] = 'n';
							break;
						case '\r':
							TRACE_ARG_APPEND(1);
							(*str)[++(*len) - l_added] = 'r';
							break;
						case '\t':
							TRACE_ARG_APPEND(1);
							(*str)[++(*len) - l_added] = 't';
							break;
						case '\f':
							TRACE_ARG_APPEND(1);
							(*str)[++(*len) - l_added] = 'f';
							break;
						case '\v':
							TRACE_ARG_APPEND(1);
							(*str)[++(*len) - l_added] = 'v';
							break;
#ifndef PHP_WIN32
						case '\e':
#else
						case VK_ESCAPE:
#endif
							TRACE_ARG_APPEND(1);
							(*str)[++(*len) - l_added] = 'e';
							break;
						case '\\':
							TRACE_ARG_APPEND(1);
							(*str)[++(*len) - l_added] = '\\';
							break;
						default:
							TRACE_ARG_APPEND(3);
							(*str)[*len - l_added + 1] = 'x';
							if ((chr >> 4) < 10) {
								(*str)[*len - l_added + 2] = (chr >> 4) + '0';
							} else {
								(*str)[*len - l_added + 2] = (chr >> 4) + 'A' - 10;
							}
							if (chr % 16 < 10) {
								(*str)[*len - l_added + 3] = chr % 16 + '0';
							} else {
								(*str)[*len - l_added + 3] = chr % 16 + 'A' - 10;
							}
							*len += 3;
					}
				}
			}
			break;
		}
		case IS_BOOL:
			if (Z_LVAL_PP(arg)) {
				TRACE_APPEND_STR("true, ");
			} else {
				TRACE_APPEND_STR("false, ");
			}
			break;
		case IS_RESOURCE:
			TRACE_APPEND_STR("Resource id #");
			/* break; */
		case IS_LONG: {
			long lval = Z_LVAL_PP(arg);
			char s_tmp[MAX_LENGTH_OF_LONG + 1];
			int l_tmp = zend_sprintf(s_tmp, "%ld", lval);  /* SAFE */
			TRACE_APPEND_STRL(s_tmp, l_tmp);
			TRACE_APPEND_STR(", ");
			break;
		}
		case IS_DOUBLE: {
			double dval = Z_DVAL_PP(arg);
			char *s_tmp;
			int l_tmp;

			s_tmp = emalloc(MAX_LENGTH_OF_DOUBLE + EG(precision) + 1);
			l_tmp = zend_sprintf(s_tmp, "%.*G", (int) EG(precision), dval);  /* SAFE */
			TRACE_APPEND_STRL(s_tmp, l_tmp);
			/* %G already handles removing trailing zeros from the fractional part, yay */
			efree(s_tmp);
			TRACE_APPEND_STR(", ");
			break;
		}
		case IS_ARRAY:
			TRACE_APPEND_STR("Array, ");
			break;
		case IS_OBJECT: {
			const char *class_name;
			zend_uint class_name_len;
			int dup;

			TRACE_APPEND_STR("Object(");

			dup = zend_get_object_classname(*arg, &class_name, &class_name_len TSRMLS_CC);

			TRACE_APPEND_STRL(class_name, class_name_len);
			if(!dup) {
				efree((char*)class_name);
			}

			TRACE_APPEND_STR("), ");
			break;
		}
		default:
			break;
	}
	return ZEND_HASH_APPLY_KEEP;
}