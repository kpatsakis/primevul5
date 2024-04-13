get_dirstack (self)
     SHELL_VAR *self;
{
  ARRAY *a;
  WORD_LIST *l;

  l = get_directory_stack (0);
  a = array_from_word_list (l);
  array_dispose (array_cell (self));
  dispose_words (l);
  var_setarray (self, a);
  return self;
}