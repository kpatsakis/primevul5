SCM_DEFINE (scm_close_fdes, "close-fdes", 1, 0, 0, 
            (SCM fd),
	    "A simple wrapper for the @code{close} system call.\n"
	    "Close file descriptor @var{fd}, which must be an integer.\n"
	    "Unlike close (@pxref{Ports and File Descriptors, close}),\n"
	    "the file descriptor will be closed even if a port is using it.\n"
	    "The return value is unspecified.")
#define FUNC_NAME s_scm_close_fdes
{
  int c_fd;
  int rv;

  c_fd = scm_to_int (fd);
  SCM_SYSCALL (rv = close (c_fd));
  if (rv < 0)
    SCM_SYSERROR;
  return SCM_UNSPECIFIED;
}