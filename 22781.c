onig_error_code_to_str(s, code, va_alist)
  UChar* s;
  int code;
  va_dcl
#endif
{
  UChar *p, *q;
  OnigErrorInfo* einfo;
  size_t len;
  int is_over;
  UChar parbuf[MAX_ERROR_PAR_LEN];
  va_list vargs;

  va_init_list(vargs, code);

  switch (code) {
  case ONIGERR_UNDEFINED_NAME_REFERENCE:
  case ONIGERR_UNDEFINED_GROUP_REFERENCE:
  case ONIGERR_MULTIPLEX_DEFINED_NAME:
  case ONIGERR_MULTIPLEX_DEFINITION_NAME_CALL:
  case ONIGERR_INVALID_GROUP_NAME:
  case ONIGERR_INVALID_CHAR_IN_GROUP_NAME:
  case ONIGERR_INVALID_CHAR_PROPERTY_NAME:
    einfo = va_arg(vargs, OnigErrorInfo*);
    len = to_ascii(einfo->enc, einfo->par, einfo->par_end,
		   parbuf, MAX_ERROR_PAR_LEN - 3, &is_over);
    q = onig_error_code_to_format(code);
    p = s;
    while (*q != '\0') {
      if (*q == '%') {
	q++;
	if (*q == 'n') { /* '%n': name */
	  xmemcpy(p, parbuf, len);
	  p += len;
	  if (is_over != 0) {
	    xmemcpy(p, "...", 3);
	    p += 3;
	  }
	  q++;
	}
	else
	  goto normal_char;
      }
      else {
      normal_char:
	*p++ = *q++;
      }
    }
    *p = '\0';
    len = p - s;
    break;

  default:
    q = onig_error_code_to_format(code);
    len = onigenc_str_bytelen_null(ONIG_ENCODING_ASCII, q);
    xmemcpy(s, q, len);
    s[len] = '\0';
    break;
  }

  va_end(vargs);
  return (int)len;
}