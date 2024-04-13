bool is_equal(const LEX_STRING *a, const LEX_STRING *b)
{
  return a->length == b->length && !strncmp(a->str, b->str, a->length);
}