setup_tmp_table_column_bitmaps(TABLE *table, uchar *bitmaps, uint field_count)
{
  uint bitmap_size= bitmap_buffer_size(field_count);

  DBUG_ASSERT(table->s->virtual_fields == 0 && table->def_vcol_set == 0);

  my_bitmap_init(&table->def_read_set, (my_bitmap_map*) bitmaps, field_count,
              FALSE);
  bitmaps+= bitmap_size;
  my_bitmap_init(&table->tmp_set,
                 (my_bitmap_map*) bitmaps, field_count, FALSE);
  bitmaps+= bitmap_size;
  my_bitmap_init(&table->eq_join_set,
                 (my_bitmap_map*) bitmaps, field_count, FALSE);
  bitmaps+= bitmap_size;
  my_bitmap_init(&table->cond_set,
                 (my_bitmap_map*) bitmaps, field_count, FALSE);
  bitmaps+= bitmap_size;
  my_bitmap_init(&table->has_value_set,
                 (my_bitmap_map*) bitmaps, field_count, FALSE);
  /* write_set and all_set are copies of read_set */
  table->def_write_set= table->def_read_set;
  table->s->all_set= table->def_read_set;
  bitmap_set_all(&table->s->all_set);
  table->default_column_bitmaps();
}