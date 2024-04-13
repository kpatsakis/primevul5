SCM_DEFINE (scm_sendfile, "sendfile", 3, 1, 0,
	    (SCM out, SCM in, SCM count, SCM offset),
	    "Send @var{count} bytes from @var{in} to @var{out}, both of which "
	    "must be either open file ports or file descriptors.  When "
	    "@var{offset} is omitted, start reading from @var{in}'s current "
	    "position; otherwise, start reading at @var{offset}.  Return "
	    "the number of bytes actually sent.")
#define FUNC_NAME s_scm_sendfile
{
#define VALIDATE_FD_OR_PORT(cvar, svar, pos)	\
  if (scm_is_integer (svar))			\
    cvar = scm_to_int (svar);			\
  else						\
    {						\
      SCM_VALIDATE_OPFPORT (pos, svar);		\
      scm_flush (svar);				\
      cvar = SCM_FPORT_FDES (svar);		\
    }

  ssize_t result SCM_UNUSED;
  size_t c_count, total = 0;
  scm_t_off c_offset;
  int in_fd, out_fd;

  VALIDATE_FD_OR_PORT (out_fd, out, 1);
  VALIDATE_FD_OR_PORT (in_fd, in, 2);
  c_count = scm_to_size_t (count);
  c_offset = SCM_UNBNDP (offset) ? 0 : scm_to_off_t (offset);

#if defined HAVE_SYS_SENDFILE_H && defined HAVE_SENDFILE
  /* The Linux-style sendfile(2), which is different from the BSD-style.  */

  {
    off_t *offset_ptr;

    offset_ptr = SCM_UNBNDP (offset) ? NULL : &c_offset;

    /* On Linux, when OUT_FD is a file, everything is transferred at once and
       RESULT == C_COUNT.  However, when OUT_FD is a pipe or other "slow"
       device, fewer bytes may be transferred, hence the loop.  RESULT == 0
       means EOF on IN_FD, so leave the loop in that case.  */
    do
      {
	result = sendfile_or_sendfile64 (out_fd, in_fd, offset_ptr,
					 c_count - total);
	if (result > 0)
	  /* At this point, either OFFSET_PTR is non-NULL and it has been
	     updated to the current offset in IN_FD, or it is NULL and IN_FD's
	     offset has been updated.  */
	  total += result;
	else if (result < 0 && (errno == EINTR || errno == EAGAIN))
	  /* Keep going.  */
	  result = 1;
      }
    while (total < c_count && result > 0);
  }

  /* Quoting the Linux man page: "In Linux kernels before 2.6.33, out_fd
     must refer to a socket.  Since Linux 2.6.33 it can be any file."
     Fall back to read(2) and write(2) when such an error occurs.  */
  if (result < 0 && errno != EINVAL && errno != ENOSYS)
    SCM_SYSERROR;
  else if (result < 0)
#endif
  {
    char buf[8192];
    size_t left;
    int reached_eof = 0;

    if (!SCM_UNBNDP (offset))
      {
	if (SCM_PORTP (in))
	  scm_seek (in, scm_from_off_t (c_offset), scm_from_int (SEEK_SET));
	else
	  {
	    if (lseek_or_lseek64 (in_fd, c_offset, SEEK_SET) < 0)
	      SCM_SYSERROR;
	  }
      }

    for (total = 0, left = c_count; total < c_count && !reached_eof; )
      {
	size_t asked, obtained, written;

	asked = SCM_MIN (sizeof buf, left);
	obtained = full_read (in_fd, buf, asked);
	if (obtained < asked)
          {
            if (errno == 0)
              reached_eof = 1;
            else
              SCM_SYSERROR;
          }

	left -= obtained;

	written = full_write (out_fd, buf, obtained);
	if (written < obtained)
	  SCM_SYSERROR;

	total += written;
      }

  }

  return scm_from_size_t (total);

#undef VALIDATE_FD_OR_PORT
}