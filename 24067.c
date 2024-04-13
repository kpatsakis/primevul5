SCM_DEFINE (scm_fcntl, "fcntl", 2, 1, 0,
            (SCM object, SCM cmd, SCM value),
	    "Apply @var{cmd} to the specified file descriptor or the underlying\n"
	    "file descriptor of the specified port.  @var{value} is an optional\n"
	    "integer argument.\n\n"
	    "Values for @var{cmd} are:\n\n"
	    "@table @code\n"
	    "@item F_DUPFD\n"
	    "Duplicate a file descriptor\n"
	    "@item F_GETFD\n"
	    "Get flags associated with the file descriptor.\n"
	    "@item F_SETFD\n"
	    "Set flags associated with the file descriptor to @var{value}.\n"
	    "@item F_GETFL\n"
	    "Get flags associated with the open file.\n"
	    "@item F_SETFL\n"
	    "Set flags associated with the open file to @var{value}\n"
	    "@item F_GETOWN\n"
	    "Get the process ID of a socket's owner, for @code{SIGIO} signals.\n"
	    "@item F_SETOWN\n"
	    "Set the process that owns a socket to @var{value}, for @code{SIGIO} signals.\n"
	    "@item FD_CLOEXEC\n"
	    "The value used to indicate the \"close on exec\" flag with @code{F_GETFL} or\n"
	    "@code{F_SETFL}.\n"
	    "@end table")
#define FUNC_NAME s_scm_fcntl
{
  int rv;
  int fdes;
  int ivalue;

  object = SCM_COERCE_OUTPORT (object);

  if (SCM_OPFPORTP (object))
    fdes = SCM_FPORT_FDES (object);
  else
    fdes = scm_to_int (object);

  if (SCM_UNBNDP (value))
    ivalue = 0;
  else
    ivalue = scm_to_int (value);

  SCM_SYSCALL (rv = fcntl (fdes, scm_to_int (cmd), ivalue));
  if (rv == -1)
    SCM_SYSERROR;
  return scm_from_int (rv);
}