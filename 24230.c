int dynamic_column_error_message(enum_dyncol_func_result rc)
{
  switch (rc) {
  case ER_DYNCOL_YES:
  case ER_DYNCOL_OK:
  case ER_DYNCOL_TRUNCATED:
    break; // it is not an error
  case ER_DYNCOL_FORMAT:
    my_error(ER_DYN_COL_WRONG_FORMAT, MYF(0));
    break;
  case ER_DYNCOL_LIMIT:
    my_error(ER_DYN_COL_IMPLEMENTATION_LIMIT, MYF(0));
    break;
  case ER_DYNCOL_RESOURCE:
    my_error(ER_OUT_OF_RESOURCES, MYF(0));
    break;
  case ER_DYNCOL_DATA:
    my_error(ER_DYN_COL_DATA, MYF(0));
    break;
  case ER_DYNCOL_UNKNOWN_CHARSET:
    my_error(ER_DYN_COL_WRONG_CHARSET, MYF(0));
    break;
  }
  return rc;
}