SCM_DEFINE (scm_closedir, "closedir", 1, 0, 0,
	    (SCM port),
	    "Close the directory stream @var{port}.\n"
	    "The return value is unspecified.")
#define FUNC_NAME s_scm_closedir
{
  SCM_VALIDATE_DIR (1, port);

  if (SCM_DIR_OPEN_P (port))
    {
      int sts;

      SCM_SYSCALL (sts = closedir ((DIR *) SCM_SMOB_DATA_1 (port)));
      if (sts != 0)
	SCM_SYSERROR;

      SCM_SET_SMOB_DATA_0 (port, scm_tc16_dir);
    }

  return SCM_UNSPECIFIED;
}