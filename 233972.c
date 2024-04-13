full_path_write (const struct url *url, char *where)
{
#define FROB(el, chr) do {                      \
  char *f_el = url->el;                         \
  if (f_el) {                                   \
    int l = strlen (f_el);                      \
    *where++ = chr;                             \
    memcpy (where, f_el, l);                    \
    where += l;                                 \
  }                                             \
} while (0)

  FROB (path, '/');
  FROB (params, ';');
  FROB (query, '?');

#undef FROB
}