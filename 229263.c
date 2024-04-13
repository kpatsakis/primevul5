int STDCALL mysql_session_track_get_next(MYSQL *mysql, enum enum_session_state_type type,
                                         const char **data, size_t *length)
{
  MYSQL_LEX_STRING *str;
  if (!mysql->extension->session_state[type].current)
    return 1;

  str= (MYSQL_LEX_STRING *)mysql->extension->session_state[type].current->data;
  mysql->extension->session_state[type].current= mysql->extension->session_state[type].current->next;

  *data= str->str ? str->str : NULL;
  *length= str->str ? str->length : 0;
  return 0;
}