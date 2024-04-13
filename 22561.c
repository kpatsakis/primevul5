onig_end(void)
{
  THREAD_ATOMIC_START;

#ifdef ONIG_DEBUG_STATISTICS
  if (!onig_is_prelude()) onig_print_statistics(stderr);
#endif

#ifdef USE_SHARED_CCLASS_TABLE
  onig_free_shared_cclass_table();
#endif

#ifdef USE_PARSE_TREE_NODE_RECYCLE
  onig_free_node_list();
#endif

  onig_inited = 0;

  THREAD_ATOMIC_END;
  THREAD_SYSTEM_END;
  return 0;
}