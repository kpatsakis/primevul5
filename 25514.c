static void free_plugin_mem(struct st_plugin_dl *p)
{
#ifdef HAVE_DLOPEN
  if (p->ptr_backup)
  {
    DBUG_ASSERT(p->nbackups);
    DBUG_ASSERT(p->handle);
    restore_ptr_backup(p->nbackups, p->ptr_backup);
    my_free(p->ptr_backup);
  }
  if (p->handle)
    dlclose(p->handle);
#endif
  my_free(p->dl.str);
  if (p->allocated)
    my_free(p->plugins);
}