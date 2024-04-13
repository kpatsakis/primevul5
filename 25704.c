bool create_internal_tmp_table(TABLE *table, KEY *keyinfo, 
                               TMP_ENGINE_COLUMNDEF *start_recinfo,
                               TMP_ENGINE_COLUMNDEF **recinfo, 
                               ulonglong options)
{
  int error;
  MARIA_KEYDEF keydef;
  MARIA_UNIQUEDEF uniquedef;
  TABLE_SHARE *share= table->s;
  MARIA_CREATE_INFO create_info;
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
      if (!share->uniques && !(keyinfo->flags & HA_NOSAME))
      {
        my_error(ER_INTERNAL_ERROR, MYF(0),
                 "Using too big key for internal temp tables");
        DBUG_RETURN(1);
      }

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
      (*recinfo)->type=   FIELD_CHECK;
      (*recinfo)->length= MARIA_UNIQUE_HASH_LENGTH;
      (*recinfo)++;
      share->reclength+=      MARIA_UNIQUE_HASH_LENGTH;
    }
    else
    {
      /* Create a key */
      bzero((char*) &keydef,sizeof(keydef));
      keydef.flag= keyinfo->flags & HA_NOSAME;
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
	seg->bit_start= (uint8)(field->pack_length() -
                                portable_sizeof_char_ptr);
	seg->flag= HA_BLOB_PART;
	seg->length=0;			// Whole blob in unique constraint
      }
      else
      {
	seg->type= keyinfo->key_part[i].type;
        /* Tell handler if it can do suffic space compression */
	if (field->real_type() == MYSQL_TYPE_STRING &&
	    keyinfo->key_part[i].length > 32)
	  seg->flag|= HA_SPACE_PACK;
      }
      if (!(field->flags & NOT_NULL_FLAG))
      {
	seg->null_bit= field->null_bit;
	seg->null_pos= (uint) (field->null_ptr - (uchar*) table->record[0]);
	/*
	  We are using a GROUP BY on something that contains NULL
	  In this case we have to tell Aria that two NULL should
	  on INSERT be regarded at the same value
	*/
	if (!using_unique_constraint)
	  keydef.flag|= HA_NULL_ARE_EQUAL;
      }
    }
  }
  bzero((char*) &create_info,sizeof(create_info));
  create_info.data_file_length= table->in_use->variables.tmp_disk_table_size;

  /*
    The logic for choosing the record format:
    The STATIC_RECORD format is the fastest one, because it's so simple,
    so we use this by default for short rows.
    BLOCK_RECORD caches both row and data, so this is generally faster than
    DYNAMIC_RECORD. The one exception is when we write to tmp table and
    want to use keys for duplicate elimination as with BLOCK RECORD
    we first write the row, then check for key conflicts and then we have to
    delete the row.  The cases when this can happen is when there is
    a group by and no sum functions or if distinct is used.
  */
  {
    enum data_file_type file_type= table->no_rows ? NO_RECORD :
        (share->reclength < 64 && !share->blob_fields ? STATIC_RECORD :
         table->used_for_duplicate_elimination ? DYNAMIC_RECORD : BLOCK_RECORD);
    uint create_flags= HA_CREATE_TMP_TABLE | HA_CREATE_INTERNAL_TABLE |
        (table->keep_row_order ? HA_PRESERVE_INSERT_ORDER : 0);

    if (file_type != NO_RECORD && encrypt_tmp_disk_tables)
    {
      /* encryption is only supported for BLOCK_RECORD */
      file_type= BLOCK_RECORD;
      if (table->used_for_duplicate_elimination)
      {
        /*
          sql-layer expect the last column to be stored/restored also
          when it's null.

          This is probably a bug (that sql-layer doesn't annotate
          the column as not-null) but both heap, aria-static, aria-dynamic and
          myisam has this property. aria-block_record does not since it
          does not store null-columns at all.
          Emulate behaviour by making column not-nullable when creating the
          table.
        */
        uint cols= (uint)(*recinfo-start_recinfo);
        start_recinfo[cols-1].null_bit= 0;
      }
    }

    if ((error= maria_create(share->path.str, file_type, share->keys, &keydef,
                             (uint) (*recinfo-start_recinfo), start_recinfo,
                             share->uniques, &uniquedef, &create_info,
                             create_flags)))
    {
      table->file->print_error(error,MYF(0));	/* purecov: inspected */
      table->db_stat=0;
      goto err;
    }
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