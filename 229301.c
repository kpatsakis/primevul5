static void ma_clear_session_state(MYSQL *mysql)
{
  uint i;

  if (!mysql || !mysql->extension)
    return;

  for (i= SESSION_TRACK_BEGIN; i <= SESSION_TRACK_END; i++)
  {
    list_free(mysql->extension->session_state[i].list, 0);
  }
  memset(mysql->extension->session_state, 0, sizeof(struct st_mariadb_session_state) * SESSION_TRACK_TYPES);
}