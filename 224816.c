set_pipestatus_array (ps, nproc)
     int *ps;
     int nproc;
{
  SHELL_VAR *v;
  ARRAY *a;
  ARRAY_ELEMENT *ae;
  register int i;
  char *t, tbuf[INT_STRLEN_BOUND(int) + 1];

  v = find_variable ("PIPESTATUS");
  if (v == 0)
    v = make_new_array_variable ("PIPESTATUS");
  if (array_p (v) == 0)
    return;		/* Do nothing if not an array variable. */
  a = array_cell (v);

  if (a == 0 || array_num_elements (a) == 0)
    {
      for (i = 0; i < nproc; i++)	/* was ps[i] != -1, not i < nproc */
	{
	  t = inttostr (ps[i], tbuf, sizeof (tbuf));
	  array_insert (a, i, t);
	}
      return;
    }

  /* Fast case */
  if (array_num_elements (a) == nproc && nproc == 1)
    {
      ae = element_forw (a->head);
      free (element_value (ae));
      ae->value = itos (ps[0]);
    }
  else if (array_num_elements (a) <= nproc)
    {
      /* modify in array_num_elements members in place, then add */
      ae = a->head;
      for (i = 0; i < array_num_elements (a); i++)
	{
	  ae = element_forw (ae);
	  free (element_value (ae));
	  ae->value = itos (ps[i]);
	}
      /* add any more */
      for ( ; i < nproc; i++)
	{
	  t = inttostr (ps[i], tbuf, sizeof (tbuf));
	  array_insert (a, i, t);
	}
    }
  else
    {
      /* deleting elements.  it's faster to rebuild the array. */	  
      array_flush (a);
      for (i = 0; ps[i] != -1; i++)
	{
	  t = inttostr (ps[i], tbuf, sizeof (tbuf));
	  array_insert (a, i, t);
	}
    }
}