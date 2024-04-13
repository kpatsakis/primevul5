bool Virtual_tmp_table::add(List<Column_definition> &field_list)
{
  /* Create all fields and calculate the total length of record */
  Column_definition *cdef;            /* column definition */
  List_iterator_fast<Column_definition> it(field_list);
  for ( ; (cdef= it++); )
  {
    Field *tmp;
    if (!(tmp= cdef->make_field(s, in_use->mem_root, 0,
                             (uchar*) (f_maybe_null(cdef->pack_flag) ? "" : 0),
                             f_maybe_null(cdef->pack_flag) ? 1 : 0,
                             cdef->field_name)))
      return true;
    add(tmp);
  }
  return false;
}