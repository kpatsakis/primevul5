field_width_error (const char *filename, const char *fieldname,
		   uintmax_t value, size_t width, bool nul)
{
  char valbuf[UINTMAX_STRSIZE_BOUND + 1];
  char maxbuf[UINTMAX_STRSIZE_BOUND + 1];
  error (0, 0, _("%s: value %s %s out of allowed range 0..%s"),
	 filename, fieldname,
	 STRINGIFY_BIGINT (value, valbuf),
	 STRINGIFY_BIGINT (MAX_VAL_WITH_DIGITS (width - nul, LG_8),
			   maxbuf));
}