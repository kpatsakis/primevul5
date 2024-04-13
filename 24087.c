SCM_DEFINE (scm_copy_file, "copy-file", 2, 0, 0,
            (SCM oldfile, SCM newfile),
	    "Copy the file specified by @var{oldfile} to @var{newfile}.\n"
	    "The return value is unspecified.")
#define FUNC_NAME s_scm_copy_file
{
  char *c_oldfile, *c_newfile;
  int oldfd, newfd;
  int n, rv;
  char buf[BUFSIZ];
  struct stat_or_stat64 oldstat;

  scm_dynwind_begin (0);
  
  c_oldfile = scm_to_locale_string (oldfile);
  scm_dynwind_free (c_oldfile);
  c_newfile = scm_to_locale_string (newfile);
  scm_dynwind_free (c_newfile);

  oldfd = open_or_open64 (c_oldfile, O_RDONLY | O_BINARY);
  if (oldfd == -1)
    SCM_SYSERROR;

  SCM_SYSCALL (rv = fstat_or_fstat64 (oldfd, &oldstat));
  if (rv == -1)
    goto err_close_oldfd;

  /* use POSIX flags instead of 07777?.  */
  newfd = open_or_open64 (c_newfile, O_WRONLY | O_CREAT | O_TRUNC,
                          oldstat.st_mode & 07777);
  if (newfd == -1)
    {
    err_close_oldfd:
      close (oldfd);
      SCM_SYSERROR;
    }

  while ((n = read (oldfd, buf, sizeof buf)) > 0)
    if (write (newfd, buf, n) != n)
      {
	close (oldfd);
	close (newfd);
	SCM_SYSERROR;
      }
  close (oldfd);
  if (close (newfd) == -1)
    SCM_SYSERROR;

  scm_dynwind_end ();
  return SCM_UNSPECIFIED;
}