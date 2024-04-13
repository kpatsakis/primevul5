long_format (struct cpio_file_stat *file_hdr, char const *link_name)
{
  char mbuf[11];
  char tbuf[40];
  time_t when;

  mode_string (file_hdr->c_mode, mbuf);
  mbuf[10] = '\0';

  /* Get time values ready to print.  */
  when = file_hdr->c_mtime;
  strcpy (tbuf, ctime (&when));
  if (current_time - when > 6L * 30L * 24L * 60L * 60L
      || current_time - when < 0L)
    {
      /* The file is older than 6 months, or in the future.
	 Show the year instead of the time of day.  */
      strcpy (tbuf + 11, tbuf + 19);
    }
  tbuf[16] = '\0';

  printf ("%s %3lu ", mbuf, (unsigned long) file_hdr->c_nlink);

  if (numeric_uid)
    printf ("%-8u %-8u ", (unsigned int) file_hdr->c_uid,
	    (unsigned int) file_hdr->c_gid);
  else
    printf ("%-8.8s %-8.8s ", getuser (file_hdr->c_uid),
	    getgroup (file_hdr->c_gid));

  if ((file_hdr->c_mode & CP_IFMT) == CP_IFCHR
      || (file_hdr->c_mode & CP_IFMT) == CP_IFBLK)
    printf ("%3lu, %3lu ",
	    (unsigned long) file_hdr->c_rdev_maj,
	    (unsigned long) file_hdr->c_rdev_min);
  else
    printf ("%8"PRIuMAX" ", (uintmax_t) file_hdr->c_filesize);

  printf ("%s ", tbuf + 4);

  printf ("%s", quotearg (file_hdr->c_name));
  if (link_name)
    {
      printf (" -> ");
      printf ("%s", quotearg (link_name));
    }
  putc ('\n', stdout);
}