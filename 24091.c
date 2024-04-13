SCM_DEFINE (scm_i_mkstemp, "mkstemp!", 1, 1, 0,
	    (SCM tmpl, SCM mode),
	    "Create a new unique file in the file system and return a new\n"
	    "buffered port open for reading and writing to the file.\n"
	    "\n"
	    "@var{tmpl} is a string specifying where the file should be\n"
	    "created: it must end with @samp{XXXXXX} and those @samp{X}s\n"
	    "will be changed in the string to return the name of the file.\n"
	    "(@code{port-filename} on the port also gives the name.)\n"
	    "\n"
	    "POSIX doesn't specify the permissions mode of the file, on GNU\n"
	    "and most systems it's @code{#o600}.  An application can use\n"
	    "@code{chmod} to relax that if desired.  For example\n"
	    "@code{#o666} less @code{umask}, which is usual for ordinary\n"
	    "file creation,\n"
	    "\n"
	    "@example\n"
	    "(let ((port (mkstemp! (string-copy \"/tmp/myfile-XXXXXX\"))))\n"
	    "  (chmod port (logand #o666 (lognot (umask))))\n"
	    "  ...)\n"
	    "@end example\n"
            "\n"
            "The optional @var{mode} argument specifies a mode, as a string\n"
            "in the same format that @code{open-file} takes.  It defaults\n"
            "to @code{\"w+\"}.")
#define FUNC_NAME s_scm_i_mkstemp
{
  char *c_tmpl;
  long mode_bits;
  int rv;
  int open_flags, is_binary;
  SCM port;

  scm_dynwind_begin (0);

  c_tmpl = scm_to_locale_string (tmpl);
  scm_dynwind_free (c_tmpl);
  if (SCM_UNBNDP (mode))
    {
      /* mkostemp will create a read/write file and add on additional
         flags; open_flags just adjoins flags to that set.  */
      open_flags = 0;
      is_binary = 0;
      mode_bits = SCM_OPN | SCM_RDNG | SCM_WRTNG;
    }
  else
    {
      open_flags = scm_i_mode_to_open_flags (mode, &is_binary, FUNC_NAME);
      mode_bits = scm_i_mode_bits (mode);
    }

  SCM_SYSCALL (rv = mkostemp (c_tmpl, open_flags));
  if (rv == -1)
    SCM_SYSERROR;

  scm_substring_move_x (scm_from_locale_string (c_tmpl),
			SCM_INUM0, scm_string_length (tmpl),
			tmpl, SCM_INUM0);

  scm_dynwind_end ();

  port = scm_i_fdes_to_port (rv, mode_bits, tmpl);
  if (is_binary)
    /* Use the binary-friendly ISO-8859-1 encoding. */
    scm_i_set_port_encoding_x (port, NULL);

  return port;
}