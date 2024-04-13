prpending (char const *lim)
{
  if (!lastout)
    lastout = bufbeg;
  while (pending > 0 && lastout < lim)
    {
      char const *nl = memchr (lastout, eolbyte, lim - lastout);
      size_t match_size;
      --pending;
      if (outleft
          || ((execute (lastout, nl + 1 - lastout,
                        &match_size, NULL) == (size_t) -1)
              == !out_invert))
        prline (lastout, nl + 1, SEP_CHAR_REJECTED);
      else
        pending = 0;
    }
}