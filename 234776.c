bool Virtual_tmp_table::open()
{
  // Make sure that we added all the fields we planned to:
  DBUG_ASSERT(s->fields == m_alloced_field_count);
  field[s->fields]= NULL;            // mark the end of the list
  s->blob_field[s->blob_fields]= 0;  // mark the end of the list

  uint null_pack_length= (s->null_fields + 7) / 8; // NULL-bit array length
  s->reclength+= null_pack_length;
  s->rec_buff_length= ALIGN_SIZE(s->reclength + 1);
  if (!(record[0]= (uchar*) in_use->alloc(s->rec_buff_length)))
    return true;
  if (null_pack_length)
  {
    null_flags= (uchar*) record[0];
    s->null_bytes= s->null_bytes_for_compare= null_pack_length;
  }
  setup_field_pointers();
  return false;
}