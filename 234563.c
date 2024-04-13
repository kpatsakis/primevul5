void vers_select_conds_t::print(String *str, enum_query_type query_type) const
{
  switch (orig_type) {
  case SYSTEM_TIME_UNSPECIFIED:
    break;
  case SYSTEM_TIME_AS_OF:
    start.print(str, query_type, STRING_WITH_LEN(" FOR SYSTEM_TIME AS OF "));
    break;
  case SYSTEM_TIME_FROM_TO:
    start.print(str, query_type, STRING_WITH_LEN(" FOR SYSTEM_TIME FROM "));
    end.print(str, query_type, STRING_WITH_LEN(" TO "));
    break;
  case SYSTEM_TIME_BETWEEN:
    start.print(str, query_type, STRING_WITH_LEN(" FOR SYSTEM_TIME BETWEEN "));
    end.print(str, query_type, STRING_WITH_LEN(" AND "));
    break;
  case SYSTEM_TIME_BEFORE:
  case SYSTEM_TIME_HISTORY:
    DBUG_ASSERT(0);
    break;
  case SYSTEM_TIME_ALL:
    str->append(" FOR SYSTEM_TIME ALL");
    break;
  }
}