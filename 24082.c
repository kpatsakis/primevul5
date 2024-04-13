SCM_DEFINE (scm_opendir, "opendir", 1, 0, 0,
	    (SCM dirname),
	    "Open the directory specified by @var{dirname} and return a directory\n"
	    "stream.")
#define FUNC_NAME s_scm_opendir
{
  DIR *ds;
  STRING_SYSCALL (dirname, c_dirname, ds = opendir (c_dirname));
  if (ds == NULL)
    SCM_SYSERROR;
  SCM_RETURN_NEWSMOB (scm_tc16_dir | (SCM_DIR_FLAG_OPEN<<16), ds);
}