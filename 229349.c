BOOL CALLBACK win_init_once(
  PINIT_ONCE InitOnce,
  PVOID Parameter,
  PVOID *lpContext)
{
  return !mysql_once_init();
  return TRUE;
}