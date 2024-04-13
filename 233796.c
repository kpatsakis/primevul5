add_count (uintmax_t a, uintmax_t b)
{
  uintmax_t sum = a + b;
  if (sum < a)
    error (EXIT_TROUBLE, 0, _("input is too large to count"));
  return sum;
}