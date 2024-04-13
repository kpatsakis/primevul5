rb_enc_cr_str_copy_for_substr(VALUE dest, VALUE src)
{
    /* this function is designed for copying encoding and coderange
     * from src to new string "dest" which is made from the part of src.
     */
    str_enc_copy(dest, src);
    switch (ENC_CODERANGE(src)) {
      case ENC_CODERANGE_7BIT:
	ENC_CODERANGE_SET(dest, ENC_CODERANGE_7BIT);
	break;
      case ENC_CODERANGE_VALID:
	if (!rb_enc_asciicompat(STR_ENC_GET(src)) ||
	    search_nonascii(RSTRING_PTR(dest), RSTRING_END(dest)))
	    ENC_CODERANGE_SET(dest, ENC_CODERANGE_VALID);
	else
	    ENC_CODERANGE_SET(dest, ENC_CODERANGE_7BIT);
	break;
      default:
	if (RSTRING_LEN(dest) == 0) {
	    if (!rb_enc_asciicompat(STR_ENC_GET(src)))
		ENC_CODERANGE_SET(dest, ENC_CODERANGE_VALID);
	    else
		ENC_CODERANGE_SET(dest, ENC_CODERANGE_7BIT);
	}
	break;
    }
}