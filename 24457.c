void SELECT_LEX::increase_derived_records(ha_rows records)
{
  SELECT_LEX_UNIT *unit= master_unit();
  DBUG_ASSERT(unit->derived);

  if (unit->with_element && unit->with_element->is_recursive)
  {
    st_select_lex *first_recursive= unit->with_element->first_recursive;
    st_select_lex *sl= unit->first_select();
    for ( ; sl != first_recursive; sl= sl->next_select())
    {
      if (sl == this)
        break;
    }
    if (sl == first_recursive)
      return; 
  }
  
  select_union *result= (select_union*)unit->result;
  if (HA_ROWS_MAX - records > result->records)
    result->records+= records;
  else
    result->records= HA_ROWS_MAX;
}