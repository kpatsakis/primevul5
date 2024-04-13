zero_block_p (char const *buffer, size_t size)
{
  while (size--)
    if (*buffer++)
      return false;
  return true;
}
