static void init_aria_psi_keys(void)
{
  const char* category= "aria";
  int count;

  count= array_elements(all_aria_mutexes);
  mysql_mutex_register(category, all_aria_mutexes, count);

  count= array_elements(all_aria_rwlocks);
  mysql_rwlock_register(category, all_aria_rwlocks, count);

  count= array_elements(all_aria_conds);
  mysql_cond_register(category, all_aria_conds, count);

  count= array_elements(all_aria_threads);
  mysql_thread_register(category, all_aria_threads, count);

  count= array_elements(all_aria_files);
  mysql_file_register(category, all_aria_files, count);
# ifdef HAVE_PSI_STAGE_INTERFACE
  count= array_elements(all_aria_stages);
  mysql_stage_register(category, all_aria_stages, count);
# endif /* HAVE_PSI_STAGE_INTERFACE */
}