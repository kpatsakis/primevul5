set_element (fd_set *set, SCM *ports_ready, SCM element, int pos)
{
  int fd;

  if (scm_is_integer (element))
    {
      fd = scm_to_int (element);
    }
  else
    {
      int use_buf = 0;

      element = SCM_COERCE_OUTPORT (element);
      SCM_ASSERT (SCM_OPFPORTP (element), element, pos, "select");
      if (pos == SCM_ARG1)
	{
	  /* check whether port has buffered input.  */
	  scm_t_port *pt = SCM_PTAB_ENTRY (element);
      
	  if (pt->read_pos < pt->read_end)
	    use_buf = 1;
	}
      else if (pos == SCM_ARG2)
	{
	  /* check whether port's output buffer has room.  */
	  scm_t_port *pt = SCM_PTAB_ENTRY (element);

	  /* > 1 since writing the last byte in the buffer causes flush.  */
	  if (pt->write_end - pt->write_pos > 1)
	    use_buf = 1;
	}
      fd = use_buf ? -1 : SCM_FPORT_FDES (element);
    }
  if (fd == -1)
    *ports_ready = scm_cons (element, *ports_ready);
  else
    FD_SET (fd, set);
  return fd;
}