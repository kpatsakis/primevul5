setup_tmp_table_column_bitmaps(TABLE *table, uchar *bitmaps)
{
  setup_tmp_table_column_bitmaps(table, bitmaps, table->s->fields);
}