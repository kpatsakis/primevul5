mkalldirs (const char *path)
{
  const char *p;
  char *t;
  struct_stat st;
  int res;

  p = path + strlen (path);
  for (; *p != '/' && p != path; p--)
    ;

  /* Don't create if it's just a file.  */
  if ((p == path) && (*p != '/'))
    return 0;
  t = strdupdelim (path, p);

  /* Check whether the directory exists.  */
  if ((stat (t, &st) == 0))
    {
      if (S_ISDIR (st.st_mode))
        {
          xfree (t);
          return 0;
        }
      else
        {
          /* If the dir exists as a file name, remove it first.  This
             is *only* for Wget to work with buggy old CERN http
             servers.  Here is the scenario: When Wget tries to
             retrieve a directory without a slash, e.g.
             http://foo/bar (bar being a directory), CERN server will
             not redirect it too http://foo/bar/ -- it will generate a
             directory listing containing links to bar/file1,
             bar/file2, etc.  Wget will lose because it saves this
             HTML listing to a file `bar', so it cannot create the
             directory.  To work around this, if the file of the same
             name exists, we just remove it and create the directory
             anyway.  */
          DEBUGP (("Removing %s because of directory danger!\n", t));
          unlink (t);
        }
    }
  res = make_directory (t);
  if (res != 0)
    logprintf (LOG_NOTQUIET, "%s: %s", t, strerror (errno));
  xfree (t);
  return res;
}