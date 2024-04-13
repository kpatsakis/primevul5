print_formatted_time (fp, format, rs, rsf, us, usf, ss, ssf, cpu)
     FILE *fp;
     char *format;
     time_t rs;
     int rsf;
     time_t us;
     int usf;
     time_t ss;
     int ssf, cpu;
{
  int prec, lng, len;
  char *str, *s, ts[INT_STRLEN_BOUND (time_t) + sizeof ("mSS.FFFF")];
  time_t sum;
  int sum_frac;
  int sindex, ssize;

  len = strlen (format);
  ssize = (len + 64) - (len % 64);
  str = (char *)xmalloc (ssize);
  sindex = 0;

  for (s = format; *s; s++)
    {
      if (*s != '%' || s[1] == '\0')
	{
	  RESIZE_MALLOCED_BUFFER (str, sindex, 1, ssize, 64);
	  str[sindex++] = *s;
	}
      else if (s[1] == '%')
	{
	  s++;
	  RESIZE_MALLOCED_BUFFER (str, sindex, 1, ssize, 64);
	  str[sindex++] = *s;
	}
      else if (s[1] == 'P')
	{
	  s++;
#if 0
	  /* clamp CPU usage at 100% */
	  if (cpu > 10000)
	    cpu = 10000;
#endif
	  sum = cpu / 100;
	  sum_frac = (cpu % 100) * 10;
	  len = mkfmt (ts, 2, 0, sum, sum_frac);
	  RESIZE_MALLOCED_BUFFER (str, sindex, len, ssize, 64);
	  strcpy (str + sindex, ts);
	  sindex += len;
	}
      else
	{
	  prec = 3;	/* default is three places past the decimal point. */
	  lng = 0;	/* default is to not use minutes or append `s' */
	  s++;
	  if (DIGIT (*s))		/* `precision' */
	    {
	      prec = *s++ - '0';
	      if (prec > 3) prec = 3;
	    }
	  if (*s == 'l')		/* `length extender' */
	    {
	      lng = 1;
	      s++;
	    }
	  if (*s == 'R' || *s == 'E')
	    len = mkfmt (ts, prec, lng, rs, rsf);
	  else if (*s == 'U')
	    len = mkfmt (ts, prec, lng, us, usf);
	  else if (*s == 'S')
	    len = mkfmt (ts, prec, lng, ss, ssf);
	  else
	    {
	      internal_error (_("TIMEFORMAT: `%c': invalid format character"), *s);
	      free (str);
	      return;
	    }
	  RESIZE_MALLOCED_BUFFER (str, sindex, len, ssize, 64);
	  strcpy (str + sindex, ts);
	  sindex += len;
	}
    }

  str[sindex] = '\0';
  fprintf (fp, "%s\n", str);
  fflush (fp);

  free (str);
}