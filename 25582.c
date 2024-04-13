bool create_internal_tmp_table(TABLE *table, KEY *keyinfo, 
                               TMP_ENGINE_COLUMNDEF *start_recinfo,
                               TMP_ENGINE_COLUMNDEF **recinfo,
                               ulonglong options)
{
  int error;
  MI_KEYDEF keydef;
  MI_UNIQUEDEF uniquedef;
  TABLE_SHARE *share= table->s;
  DBUG_ENTER("create_internal_tmp_table");

  if (share->keys)
  {						// Get keys for ni_create
    bool using_unique_constraint=0;
    HA_KEYSEG *seg= (HA_KEYSEG*) alloc_root(&table->mem_root,
                                            sizeof(*seg) * keyinfo->user_defined_key_parts);
    if (!seg)
      goto err;

    bzero(seg, sizeof(*seg) * keyinfo->user_defined_key_parts);
    /*
       Note that a similar check is performed during
       subquery_types_allow_materialization. See MDEV-7122 for more details as
       to why. Whenever this changes, it must be updated there as well, for
       all tmp_table engines.
    */
    if (keyinfo->key_length > table->file->max_key_length() ||
	keyinfo->user_defined_key_parts > table->file->max_key_parts() ||
	share->uniques)
    {
      /* Can't create a key; Make a unique constraint instead of a key */
      share->keys=    0;
      share->uniques= 1;
      using_unique_constraint=1;
      bzero((char*) &uniquedef,sizeof(uniquedef));
      uniquedef.keysegs=keyinfo->user_defined_key_parts;
      uniquedef.seg=seg;
      uniquedef.null_are_equal=1;

      /* Create extra column for hash value */
      bzero((uchar*) *recinfo,sizeof(**recinfo));
      (*recinfo)->type= FIELD_CHECK;
      (*recinfo)->length=MI_UNIQUE_HASH_LENGTH;
      (*recinfo)++;
      share->reclength+=MI_UNIQUE_HASH_LENGTH;
    }
    else
    {
      /* Create an unique key */
      bzero((char*) &keydef,sizeof(keydef));
      keydef.flag= ((keyinfo->flags & HA_NOSAME) | HA_BINARY_PACK_KEY |
                    HA_PACK_KEY);
      keydef.keysegs=  keyinfo->user_defined_key_parts;
      keydef.seg= seg;
    }
    for (uint i=0; i < keyinfo->user_defined_key_parts ; i++,seg++)
    {
      Field *field=keyinfo->key_part[i].field;
      seg->flag=     0;
      seg->language= field->charset()->number;
      seg->length=   keyinfo->key_part[i].length;
      seg->start=    keyinfo->key_part[i].offset;
      if (field->flags & BLOB_FLAG)
      {
	seg->type=
	((keyinfo->key_part[i].key_type & FIELDFLAG_BINARY) ?
	 HA_KEYTYPE_VARBINARY2 : HA_KEYTYPE_VARTEXT2);
	seg->bit_start= (uint8)(field->pack_length() - portable_sizeof_char_ptr);
	seg->flag= HA_BLOB_PART;
	seg->length=0;			// Whole blob in unique constraint
      }
      else
      {
	seg->type= keyinfo->key_part[i].type;
        /* Tell handler if it can do suffic space compression */
	if (field->real_type() == MYSQL_TYPE_STRING &&
	    keyinfo->key_part[i].length > 4)
	  seg->flag|= HA_SPACE_PACK;
      }
      if (!(field->flags & NOT_NULL_FLAG))
      {
	seg->null_bit= field->null_bit;
	seg->null_pos= (uint) (field->null_ptr - (uchar*) table->record[0]);
	/*
	  We are using a GROUP BY on something that contains NULL
	  In this case we have to tell MyISAM that two NULL should
	  on INSERT be regarded at the same value
	*/
	if (!using_unique_constraint)
	  keydef.flag|= HA_NULL_ARE_EQUAL;
      }
    }
  }
  MI_CREATE_INFO create_info;
  bzero((char*) &create_info,sizeof(create_info));
  create_info.data_file_length= table->in_use->variables.tmp_disk_table_size;

  if ((error=mi_create(share->path.str, share->keys, &keydef,
		       (uint) (*recinfo-start_recinfo), start_recinfo,
		       share->uniques, &uniquedef, &create_info,
		       HA_CREATE_TMP_TABLE | HA_CREATE_INTERNAL_TABLE |
                       ((share->db_create_options & HA_OPTION_PACK_RECORD) ?
                        HA_PACK_RECORD : 0)
                      )))
  {
    table->file->print_error(error,MYF(0));	/* purecov: inspected */
    table->db_stat=0;
    goto err;
  }
  table->in_use->inc_status_created_tmp_disk_tables();
  table->in_use->inc_status_created_tmp_tables();
  table->in_use->query_plan_flags|= QPLAN_TMP_DISK;
  share->db_record_offset= 1;
  table->set_created();
  DBUG_RETURN(0);
 err:
  DBUG_RETURN(1);
}