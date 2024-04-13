bool JOIN::change_result(select_result *new_result, select_result *old_result)
{
  DBUG_ENTER("JOIN::change_result");
  if (old_result == NULL || result == old_result)
  {
    result= new_result;
    if (result->prepare(fields_list, select_lex->master_unit()) ||
        result->prepare2())
      DBUG_RETURN(true); /* purecov: inspected */
    DBUG_RETURN(false);
  }
  DBUG_RETURN(result->change_result(new_result));
}