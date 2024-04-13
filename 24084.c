SCM_DEFINE (scm_access, "access?", 2, 0, 0,
            (SCM path, SCM how),
	    "Test accessibility of a file under the real UID and GID of the\n"
	    "calling process.  The return is @code{#t} if @var{path} exists\n"
	    "and the permissions requested by @var{how} are all allowed, or\n"
	    "@code{#f} if not.\n"
	    "\n"
	    "@var{how} is an integer which is one of the following values,\n"
	    "or a bitwise-OR (@code{logior}) of multiple values.\n"
	    "\n"
	    "@defvar R_OK\n"
	    "Test for read permission.\n"
	    "@end defvar\n"
	    "@defvar W_OK\n"
	    "Test for write permission.\n"
	    "@end defvar\n"
	    "@defvar X_OK\n"
	    "Test for execute permission.\n"
	    "@end defvar\n"
	    "@defvar F_OK\n"
	    "Test for existence of the file.  This is implied by each of the\n"
	    "other tests, so there's no need to combine it with them.\n"
	    "@end defvar\n"
	    "\n"
	    "It's important to note that @code{access?} does not simply\n"
	    "indicate what will happen on attempting to read or write a\n"
	    "file.  In normal circumstances it does, but in a set-UID or\n"
	    "set-GID program it doesn't because @code{access?} tests the\n"
	    "real ID, whereas an open or execute attempt uses the effective\n"
	    "ID.\n"
	    "\n"
	    "A program which will never run set-UID/GID can ignore the\n"
	    "difference between real and effective IDs, but for maximum\n"
	    "generality, especially in library functions, it's best not to\n"
	    "use @code{access?} to predict the result of an open or execute,\n"
	    "instead simply attempt that and catch any exception.\n"
	    "\n"
	    "The main use for @code{access?} is to let a set-UID/GID program\n"
	    "determine what the invoking user would have been allowed to do,\n"
	    "without the greater (or perhaps lesser) privileges afforded by\n"
	    "the effective ID.  For more on this, see ``Testing File\n"
	    "Access'' in The GNU C Library Reference Manual.")
#define FUNC_NAME s_scm_access
{
  int rv;
  char *c_path;

  c_path = scm_to_locale_string (path);
  rv = access (c_path, scm_to_int (how));
  free (c_path);

  return scm_from_bool (!rv);
}