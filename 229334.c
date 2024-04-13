int STDCALL mysql_session_track_get_first(MYSQL *mysql, enum enum_session_state_type type,
                                          const char **data, size_t *length)
{
  mysql->extension->session_state[type].current= mysql->extension->session_state[type].list;
  return mysql_session_track_get_next(mysql, type, data, length);
}