static int rr_from_cache(READ_RECORD *info)
{
  uint i;
  ulong length;
  my_off_t rest_of_file;
  int16 error;
  uchar *position,*ref_position,*record_pos;
  ulong record;

  for (;;)
  {
    if (info->cache_pos != info->cache_end)
    {
      if (info->cache_pos[info->error_offset])
      {
	shortget(error,info->cache_pos);
	if (info->print_error)
	  info->table->file->print_error(error,MYF(0));
      }
      else
      {
	error=0;
	memcpy(info->record,info->cache_pos,
               (size_t) info->table->s->reclength);
      }
      info->cache_pos+=info->reclength;
      return ((int) error);
    }
    length=info->rec_cache_size;
    rest_of_file=info->io_cache->end_of_file - my_b_tell(info->io_cache);
    if ((my_off_t) length > rest_of_file)
      length= (ulong) rest_of_file;
    if (!length || my_b_read(info->io_cache,info->cache,length))
    {
      DBUG_PRINT("info",("Found end of file"));
      return -1;			/* End of file */
    }

    length/=info->ref_length;
    position=info->cache;
    ref_position=info->read_positions;
    for (i=0 ; i < length ; i++,position+=info->ref_length)
    {
      memcpy(ref_position,position,(size_t) info->ref_length);
      ref_position+=MAX_REFLENGTH;
      int3store(ref_position,(long) i);
      ref_position+=3;
    }
    my_qsort(info->read_positions, length, info->struct_length,
             (qsort_cmp) rr_cmp);

    position=info->read_positions;
    for (i=0 ; i < length ; i++)
    {
      memcpy(info->ref_pos,position,(size_t) info->ref_length);
      position+=MAX_REFLENGTH;
      record=uint3korr(position);
      position+=3;
      record_pos=info->cache+record*info->reclength;
      if ((error=(int16) info->table->file->ha_rnd_pos(record_pos,info->ref_pos)))
      {
	record_pos[info->error_offset]=1;
	shortstore(record_pos,error);
	DBUG_PRINT("error",("Got error: %d:%d when reading row",
			    my_errno, error));
      }
      else
	record_pos[info->error_offset]=0;
    }
    info->cache_end=(info->cache_pos=info->cache)+length*info->reclength;
  }
} /* rr_from_cache */