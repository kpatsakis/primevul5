retrieve_select_type (fd_set *set, SCM ports_ready, SCM list_or_vec)
{
  SCM answer_list = ports_ready;

  if (scm_is_simple_vector (list_or_vec))
    {
      int i = SCM_SIMPLE_VECTOR_LENGTH (list_or_vec);

      while (--i >= 0)
	{
	  answer_list = get_element (set,
				     SCM_SIMPLE_VECTOR_REF (list_or_vec, i),
				     answer_list);
	}
      return scm_vector (answer_list);
    }
  else
    {
      /* list_or_vec must be a list.  */
      while (!SCM_NULL_OR_NIL_P (list_or_vec))
	{
	  answer_list = get_element (set, SCM_CAR (list_or_vec), answer_list);
	  list_or_vec = SCM_CDR (list_or_vec);
	}
      return answer_list;
    }
}