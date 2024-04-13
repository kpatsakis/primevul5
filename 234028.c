ds_resize (dynamic_string *string)
{
  if (string->ds_idx == string->ds_size)
    {
      string->ds_string = x2nrealloc (string->ds_string, &string->ds_size,
				      1);
    }
}