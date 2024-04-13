SCM_DEFINE (scm_link, "link", 2, 0, 0,
            (SCM oldpath, SCM newpath),
	    "Creates a new name @var{newpath} in the file system for the\n"
	    "file named by @var{oldpath}.  If @var{oldpath} is a symbolic\n"
	    "link, the link may or may not be followed depending on the\n"
	    "system.")
#define FUNC_NAME s_scm_link
{
  int val;

  STRING2_SYSCALL (oldpath, c_oldpath,
		   newpath, c_newpath,
		   val = link (c_oldpath, c_newpath));
  if (val != 0)
    SCM_SYSERROR;
  return SCM_UNSPECIFIED;
}