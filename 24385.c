ha_rows st_select_lex::get_offset()
{
  ulonglong val= 0;

  if (offset_limit)
  {
    // see comment for st_select_lex::get_limit()
    bool fix_fields_successful= true;
    if (!offset_limit->fixed)
    {
      fix_fields_successful= !offset_limit->fix_fields(master_unit()->thd,
                                                       NULL);

      DBUG_ASSERT(fix_fields_successful);
    }
    val= fix_fields_successful ? offset_limit->val_uint() : HA_POS_ERROR;
  }

  return (ha_rows)val;
}