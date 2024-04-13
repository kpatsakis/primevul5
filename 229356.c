int STDCALL mysql_server_init(int argc __attribute__((unused)),
  char **argv __attribute__((unused)),
  char **groups __attribute__((unused)))
{
#ifdef _WIN32
  BOOL ret = InitOnceExecuteOnce(&init_once, win_init_once, NULL, NULL);
  return ret? 0: 1;
#else
  return pthread_once(&init_once, mysql_once_init);
#endif
}