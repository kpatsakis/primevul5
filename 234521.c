void JOIN_TAB::calc_used_field_length(bool max_fl)
{
  uint null_fields,blobs,fields;
  ulong rec_length;
  Field **f_ptr,*field;
  uint uneven_bit_fields;
  MY_BITMAP *read_set= table->read_set;

  uneven_bit_fields= null_fields= blobs= fields= rec_length=0;
  for (f_ptr=table->field ; (field= *f_ptr) ; f_ptr++)
  {
    if (bitmap_is_set(read_set, field->field_index))
    {
      uint flags=field->flags;
      fields++;
      rec_length+=field->pack_length();
      if (flags & BLOB_FLAG)
	blobs++;
      if (!(flags & NOT_NULL_FLAG))
	null_fields++;
      if (field->type() == MYSQL_TYPE_BIT &&
          ((Field_bit*)field)->bit_len)
        uneven_bit_fields++;
    }
  }
  if (null_fields || uneven_bit_fields)
    rec_length+=(table->s->null_fields+7)/8;
  if (table->maybe_null)
    rec_length+=sizeof(my_bool);

  /* Take into account that DuplicateElimination may need to store rowid */
  uint rowid_add_size= 0;
  if (keep_current_rowid)
  {
    rowid_add_size= table->file->ref_length; 
    rec_length += rowid_add_size;
    fields++;
  }

  if (max_fl)
  {
    // TODO: to improve this estimate for max expected length 
    if (blobs)
    {
      ulong blob_length= table->file->stats.mean_rec_length;
      if (ULONG_MAX - rec_length > blob_length)
        rec_length+=  blob_length;
      else
        rec_length= ULONG_MAX;
    }
    max_used_fieldlength= rec_length;
  } 
  else if (table->file->stats.mean_rec_length)
    set_if_smaller(rec_length, table->file->stats.mean_rec_length + rowid_add_size);
      
  used_fields=fields;
  used_fieldlength=rec_length;
  used_blobs=blobs;
  used_null_fields= null_fields;
  used_uneven_bit_fields= uneven_bit_fields;
}