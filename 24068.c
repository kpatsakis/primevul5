get_element (fd_set *set, SCM element, SCM list)
{
  int fd;

  if (scm_is_integer (element))
    {
      fd = scm_to_int (element);
    }
  else
    {
      fd = SCM_FPORT_FDES (SCM_COERCE_OUTPORT (element));
    }
  if (FD_ISSET (fd, set))
    list = scm_cons (element, list);
  return list;
}