print_select_list (list, list_len, max_elem_len, indices_len)
     WORD_LIST *list;
     int list_len, max_elem_len, indices_len;
{
  int ind, row, elem_len, pos, cols, rows;
  int first_column_indices_len, other_indices_len;

  if (list == 0)
    {
      putc ('\n', stderr);
      return;
    }

  cols = max_elem_len ? COLS / max_elem_len : 1;
  if (cols == 0)
    cols = 1;
  rows = list_len ? list_len / cols + (list_len % cols != 0) : 1;
  cols = list_len ? list_len / rows + (list_len % rows != 0) : 1;

  if (rows == 1)
    {
      rows = cols;
      cols = 1;
    }

  first_column_indices_len = NUMBER_LEN (rows);
  other_indices_len = indices_len;

  for (row = 0; row < rows; row++)
    {
      ind = row;
      pos = 0;
      while (1)
	{
	  indices_len = (pos == 0) ? first_column_indices_len : other_indices_len;
	  elem_len = print_index_and_element (indices_len, ind + 1, list);
	  elem_len += indices_len + RP_SPACE_LEN;
	  ind += rows;
	  if (ind >= list_len)
	    break;
	  indent (pos + elem_len, pos + max_elem_len);
	  pos += max_elem_len;
	}
      putc ('\n', stderr);
    }
}