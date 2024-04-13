static int as_is_first(const struct host_query* hquery)
{
  char* p;
  int ndots = 0;
  for (p = hquery->name; *p; p++)
    {
      if (*p == '.')
        {
          ndots++;
        }
    }
  return ndots >= hquery->channel->ndots;
}