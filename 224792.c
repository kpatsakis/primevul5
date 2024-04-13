sv_histchars (name)
     char *name;
{
  char *temp;

  temp = get_string_value (name);
  if (temp)
    {
      history_expansion_char = *temp;
      if (temp[0] && temp[1])
	{
	  history_subst_char = temp[1];
	  if (temp[2])
	      history_comment_char = temp[2];
	}
    }
  else
    {
      history_expansion_char = '!';
      history_subst_char = '^';
      history_comment_char = '#';
    }
}