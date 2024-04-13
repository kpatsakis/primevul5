indent (from, to)
     int from, to;
{
  while (from < to)
    {
      if ((to / tabsize) > (from / tabsize))
	{
	  putc ('\t', stderr);
	  from += tabsize - from % tabsize;
	}
      else
	{
	  putc (' ', stderr);
	  from++;
	}
    }
}