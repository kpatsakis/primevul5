bool Virtual_tmp_table::add(List<Spvar_definition> &field_list)
{
  /* Create all fields and calculate the total length of record */
  Spvar_definition *cdef;            /* column definition */
  List_iterator_fast<Spvar_definition> it(field_list);
  DBUG_ENTER("Virtual_tmp_table::add");
  while ((cdef= it++))
  {
    Field *tmp;
    if (!(tmp= cdef->make_field(s, in_use->mem_root, 0,
                             (uchar*) (f_maybe_null(cdef->pack_flag) ? "" : 0),
                             f_maybe_null(cdef->pack_flag) ? 1 : 0,
                             &cdef->field_name)))
      DBUG_RETURN(true);
     add(tmp);
  }
  DBUG_RETURN(false);
}