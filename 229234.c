describe_dict_fn (const char * const lang,
                  const char * const name,
                  const char * const desc,
                  const char * const file,
                  void * ud) /* {{{ */
{
	zval *zdesc = (zval *) ud;
	array_init(zdesc);
	add_assoc_string(zdesc, "lang", (char *)lang, 1);
	add_assoc_string(zdesc, "name", (char *)name, 1);
	add_assoc_string(zdesc, "desc", (char *)desc, 1);
	add_assoc_string(zdesc, "file", (char *)file, 1);
}