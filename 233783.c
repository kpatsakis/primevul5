do_execute (char const *buf, size_t size, size_t *match_size, char const *start_ptr)
{
  size_t result;
  const char *line_next;

  /* With the current implementation, using --ignore-case with a multi-byte
     character set is very inefficient when applied to a large buffer
     containing many matches.  We can avoid much of the wasted effort
     by matching line-by-line.

     FIXME: this is just an ugly workaround, and it doesn't really
     belong here.  Also, PCRE is always using this same per-line
     matching algorithm.  Either we fix -i, or we should refactor
     this code---for example, we could add another function pointer
     to struct matcher to split the buffer passed to execute.  It would
     perform the memchr if line-by-line matching is necessary, or just
     return buf + size otherwise.  */
  if (MB_CUR_MAX == 1 || !match_icase)
    return execute (buf, size, match_size, start_ptr);

  for (line_next = buf; line_next < buf + size; )
    {
      const char *line_buf = line_next;
      const char *line_end = memchr (line_buf, eolbyte, (buf + size) - line_buf);
      if (line_end == NULL)
        line_next = line_end = buf + size;
      else
        line_next = line_end + 1;

      if (start_ptr && start_ptr >= line_end)
        continue;

      result = execute (line_buf, line_next - line_buf, match_size, start_ptr);
      if (result != (size_t) -1)
        return (line_buf - buf) + result;
    }

  return (size_t) -1;
}