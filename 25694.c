bool Virtual_tmp_table::init(uint field_count)
{
  uint *blob_field;
  uchar *bitmaps;
  if (!multi_alloc_root(in_use->mem_root,
                        &s, sizeof(*s),
                        &field, (field_count + 1) * sizeof(Field*),
                        &blob_field, (field_count + 1) * sizeof(uint),
                        &bitmaps, bitmap_buffer_size(field_count) * 6,
                        NullS))
    return true;
  s->reset();
  s->blob_field= blob_field;
  setup_tmp_table_column_bitmaps(this, bitmaps, field_count);
  m_alloced_field_count= field_count;
  return false;
};