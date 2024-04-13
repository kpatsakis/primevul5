mkfmt (buf, prec, lng, sec, sec_fraction)
     char *buf;
     int prec, lng;
     time_t sec;
     int sec_fraction;
{
  time_t min;
  char abuf[INT_STRLEN_BOUND(time_t) + 1];
  int ind, aind;

  ind = 0;
  abuf[sizeof(abuf) - 1] = '\0';

  /* If LNG is non-zero, we want to decompose SEC into minutes and seconds. */
  if (lng)
    {
      min = sec / 60;
      sec %= 60;
      aind = sizeof(abuf) - 2;
      do
	abuf[aind--] = (min % 10) + '0';
      while (min /= 10);
      aind++;
      while (abuf[aind])
	buf[ind++] = abuf[aind++];
      buf[ind++] = 'm';
    }

  /* Now add the seconds. */
  aind = sizeof (abuf) - 2;
  do
    abuf[aind--] = (sec % 10) + '0';
  while (sec /= 10);
  aind++;
  while (abuf[aind])
    buf[ind++] = abuf[aind++];

  /* We want to add a decimal point and PREC places after it if PREC is
     nonzero.  PREC is not greater than 3.  SEC_FRACTION is between 0
     and 999. */
  if (prec != 0)
    {
      buf[ind++] = '.';
      for (aind = 1; aind <= prec; aind++)
	{
	  buf[ind++] = (sec_fraction / precs[aind]) + '0';
	  sec_fraction %= precs[aind];
	}
    }

  if (lng)
    buf[ind++] = 's';
  buf[ind] = '\0';

  return (ind);
}