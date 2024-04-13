SCM_DEFINE (scm_directory_stream_p, "directory-stream?", 1, 0, 0,
	    (SCM obj),
	    "Return a boolean indicating whether @var{obj} is a directory\n"
	    "stream as returned by @code{opendir}.")
#define FUNC_NAME s_scm_directory_stream_p
{
  return scm_from_bool (SCM_DIRP (obj));
}