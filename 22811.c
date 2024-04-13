koi8_r_is_code_ctype(OnigCodePoint code, unsigned int ctype,
		     OnigEncoding enc ARG_UNUSED)
{
  if (code < 256)
    return ENC_IS_KOI8_R_CTYPE(code, ctype);
  else
    return FALSE;
}