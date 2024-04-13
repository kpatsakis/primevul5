rb_str_comparable(VALUE str1, VALUE str2)
{
    int idx1, idx2;
    int rc1, rc2;

    if (RSTRING_LEN(str1) == 0) return TRUE;
    if (RSTRING_LEN(str2) == 0) return TRUE;
    idx1 = ENCODING_GET(str1);
    idx2 = ENCODING_GET(str2);
    if (idx1 == idx2) return TRUE;
    rc1 = rb_enc_str_coderange(str1);
    rc2 = rb_enc_str_coderange(str2);
    if (rc1 == ENC_CODERANGE_7BIT) {
	if (rc2 == ENC_CODERANGE_7BIT) return TRUE;
	if (rb_enc_asciicompat(rb_enc_from_index(idx2)))
	    return TRUE;
    }
    if (rc2 == ENC_CODERANGE_7BIT) {
	if (rb_enc_asciicompat(rb_enc_from_index(idx1)))
	    return TRUE;
    }
    return FALSE;
}