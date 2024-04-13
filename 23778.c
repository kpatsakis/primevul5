tr (char const *trans, char c)
{
  return trans ? trans[U(c)] : c;
}