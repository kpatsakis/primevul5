SCM_DEFINE (scm_rewinddir, "rewinddir", 1, 0, 0,
	    (SCM port),
	    "Reset the directory port @var{port} so that the next call to\n"
	    "@code{readdir} will return the first directory entry.")
#define FUNC_NAME s_scm_rewinddir
{
  SCM_VALIDATE_DIR (1, port);
  if (!SCM_DIR_OPEN_P (port))
    SCM_MISC_ERROR ("Directory ~S is not open.", scm_list_1 (port));

  rewinddir ((DIR *) SCM_SMOB_DATA_1 (port));

  return SCM_UNSPECIFIED;
}