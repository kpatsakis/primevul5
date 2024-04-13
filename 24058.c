SCM_DEFINE (scm_system_file_name_convention,
            "system-file-name-convention", 0, 0, 0, (void),
	    "Return either @code{posix} or @code{windows}, depending on\n"
            "what kind of system this Guile is running on.")
#define FUNC_NAME s_scm_system_file_name_convention
{
  return sym_file_name_convention;
}