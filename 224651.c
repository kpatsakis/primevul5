getinterp (sample, sample_len, endp)
     char *sample;
     int sample_len, *endp;
{
  register int i;
  char *execname;
  int start;

  /* Find the name of the interpreter to exec. */
  for (i = 2; i < sample_len && whitespace (sample[i]); i++)
    ;

  for (start = i; STRINGCHAR(i); i++)
    ;

  execname = substring (sample, start, i);

  if (endp)
    *endp = i;
  return execname;
}