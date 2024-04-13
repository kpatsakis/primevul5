SCM_DEFINE (scm_stat, "stat", 1, 1, 0, 
            (SCM object, SCM exception_on_error),
	    "Return an object containing various information about the file\n"
	    "determined by @var{object}.  @var{object} can be a string containing\n"
	    "a file name or a port or integer file descriptor which is open\n"
	    "on a file (in which case @code{fstat} is used as the underlying\n"
	    "system call).\n"
	    "\n"
            "If the optional @var{exception_on_error} argument is true, which\n"
            "is the default, an exception will be raised if the underlying\n"
            "system call returns an error, for example if the file is not\n"
            "found or is not readable. Otherwise, an error will cause\n"
            "@code{stat} to return @code{#f}."
	    "\n"
	    "The object returned by a successful call to @code{stat} can be\n"
            "passed as a single parameter to the following procedures, all of\n"
            "which return integers:\n"
	    "\n"
	    "@table @code\n"
	    "@item stat:dev\n"
	    "The device containing the file.\n"
	    "@item stat:ino\n"
	    "The file serial number, which distinguishes this file from all\n"
	    "other files on the same device.\n"
	    "@item stat:mode\n"
	    "The mode of the file.  This includes file type information and\n"
	    "the file permission bits.  See @code{stat:type} and\n"
	    "@code{stat:perms} below.\n"
	    "@item stat:nlink\n"
	    "The number of hard links to the file.\n"
	    "@item stat:uid\n"
	    "The user ID of the file's owner.\n"
	    "@item stat:gid\n"
	    "The group ID of the file.\n"
	    "@item stat:rdev\n"
	    "Device ID; this entry is defined only for character or block\n"
	    "special files.\n"
	    "@item stat:size\n"
	    "The size of a regular file in bytes.\n"
	    "@item stat:atime\n"
	    "The last access time for the file.\n"
	    "@item stat:mtime\n"
	    "The last modification time for the file.\n"
	    "@item stat:ctime\n"
	    "The last modification time for the attributes of the file.\n"
	    "@item stat:blksize\n"
	    "The optimal block size for reading or writing the file, in\n"
	    "bytes.\n"
	    "@item stat:blocks\n"
	    "The amount of disk space that the file occupies measured in\n"
	    "units of 512 byte blocks.\n"
	    "@end table\n"
	    "\n"
	    "In addition, the following procedures return the information\n"
	    "from stat:mode in a more convenient form:\n"
	    "\n"
	    "@table @code\n"
	    "@item stat:type\n"
	    "A symbol representing the type of file.  Possible values are\n"
	    "regular, directory, symlink, block-special, char-special, fifo,\n"
	    "socket and unknown\n"
	    "@item stat:perms\n"
	    "An integer representing the access permission bits.\n"
	    "@end table")
#define FUNC_NAME s_scm_stat
{
  int rv;
  int fdes;
  struct stat_or_stat64 stat_temp;

  if (scm_is_integer (object))
    {
      SCM_SYSCALL (rv = fstat_or_fstat64 (scm_to_int (object), &stat_temp));
    }
  else if (scm_is_string (object))
    {
      char *file = scm_to_locale_string (object);
      SCM_SYSCALL (rv = stat_or_stat64 (file, &stat_temp));
      free (file);
    }
  else
    {
      object = SCM_COERCE_OUTPORT (object);
      SCM_VALIDATE_OPFPORT (1, object);
      fdes = SCM_FPORT_FDES (object);
      SCM_SYSCALL (rv = fstat_or_fstat64 (fdes, &stat_temp));
    }

  if (rv == -1)
    {
      if (SCM_UNBNDP (exception_on_error) || scm_is_true (exception_on_error))
        {
          int en = errno;
          SCM_SYSERROR_MSG ("~A: ~S",
                            scm_list_2 (scm_strerror (scm_from_int (en)),
                                        object),
                            en);
        }
      else
        return SCM_BOOL_F;
    }
  return scm_stat2scm (&stat_temp);
}