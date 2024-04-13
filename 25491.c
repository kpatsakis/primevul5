static void restore_ptr_backup(uint n, st_ptr_backup *backup)
{
  while (n--)
    (backup++)->restore();
}