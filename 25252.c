inline void TABLE::use_all_stored_columns()
{
  bitmap_set_all(read_set);
  if (Field **vf= vfield)
    for (; *vf; vf++)
      bitmap_clear_bit(read_set, (*vf)->field_index);
}