static bool check_request_str(const char* const str,
                              bool (*check) (int c))
{
  for (size_t i(0); str[i] != '\0'; ++i)
  {
    if (!check(str[i]))
    {
      WSREP_WARN("Illegal character in state transfer request: %i (%c).",
                 str[i], str[i]);
      return true;
    }
  }

  return false;
}