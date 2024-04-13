fill_select_type (fd_set *set, SCM *ports_ready, SCM list_or_vec, int pos)
{
  int max_fd = 0;

  if (scm_is_simple_vector (list_or_vec))
    {
      int i = SCM_SIMPLE_VECTOR_LENGTH (list_or_vec);
      
      while (--i >= 0)
	{
	  int fd = set_element (set, ports_ready,
				SCM_SIMPLE_VECTOR_REF (list_or_vec, i), pos);

	  if (fd > max_fd)
	    max_fd = fd;
	}
    }
  else
    {
      while (!SCM_NULL_OR_NIL_P (list_or_vec))
	{
	  int fd = set_element (set, ports_ready, SCM_CAR (list_or_vec), pos);

	  if (fd > max_fd)
	    max_fd = fd;
	  list_or_vec = SCM_CDR (list_or_vec);
	}
    }

  return max_fd;
}