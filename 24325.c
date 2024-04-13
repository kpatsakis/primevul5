static int init_rr_cache(THD *thd, READ_RECORD *info)
{
  uint rec_cache_size;
  DBUG_ENTER("init_rr_cache");

  info->struct_length= 3+MAX_REFLENGTH;
  info->reclength= ALIGN_SIZE(info->table->s->reclength+1);
  if (info->reclength < info->struct_length)
    info->reclength= ALIGN_SIZE(info->struct_length);

  info->error_offset= info->table->s->reclength;
  info->cache_records= (thd->variables.read_rnd_buff_size /
                        (info->reclength+info->struct_length));
  rec_cache_size= info->cache_records*info->reclength;
  info->rec_cache_size= info->cache_records*info->ref_length;

  // We have to allocate one more byte to use uint3korr (see comments for it)
  if (info->cache_records <= 2 ||
      !(info->cache=(uchar*) my_malloc_lock(rec_cache_size+info->cache_records*
					   info->struct_length+1,
					   MYF(MY_THREAD_SPECIFIC))))
    DBUG_RETURN(1);
#ifdef HAVE_valgrind
  // Avoid warnings in qsort
  bzero(info->cache,rec_cache_size+info->cache_records* info->struct_length+1);
#endif
  DBUG_PRINT("info",("Allocated buffert for %d records",info->cache_records));
  info->read_positions=info->cache+rec_cache_size;
  info->cache_pos=info->cache_end=info->cache;
  DBUG_RETURN(0);
} /* init_rr_cache */