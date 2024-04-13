bool TABLE_REF::tmp_table_index_lookup_init(THD *thd,
                                            KEY *tmp_key,
                                            Item_iterator &it,
                                            bool value,
                                            uint skip)
{
  uint tmp_key_parts= tmp_key->user_defined_key_parts;
  uint i;
  DBUG_ENTER("TABLE_REF::tmp_table_index_lookup_init");

  key= 0; /* The only temp table index. */
  key_length= tmp_key->key_length;
  if (!(key_buff=
        (uchar*) thd->calloc(ALIGN_SIZE(tmp_key->key_length) * 2)) ||
      !(key_copy=
        (store_key**) thd->alloc((sizeof(store_key*) *
                                  (tmp_key_parts + 1)))) ||
      !(items=
        (Item**) thd->alloc(sizeof(Item*) * tmp_key_parts)))
    DBUG_RETURN(TRUE);

  key_buff2= key_buff + ALIGN_SIZE(tmp_key->key_length);

  KEY_PART_INFO *cur_key_part= tmp_key->key_part;
  store_key **ref_key= key_copy;
  uchar *cur_ref_buff= key_buff;

  it.open();
  for (i= 0; i < skip; i++) it.next();
  for (i= 0; i < tmp_key_parts; i++, cur_key_part++, ref_key++)
  {
    Item *item= it.next();
    DBUG_ASSERT(item);
    items[i]= item;
    int null_count= MY_TEST(cur_key_part->field->real_maybe_null());
    *ref_key= new store_key_item(thd, cur_key_part->field,
                                 /* TIMOUR:
                                    the NULL byte is taken into account in
                                    cur_key_part->store_length, so instead of
                                    cur_ref_buff + MY_TEST(maybe_null), we could
                                    use that information instead.
                                 */
                                 cur_ref_buff + null_count,
                                 null_count ? cur_ref_buff : 0,
                                 cur_key_part->length, items[i], value);
    cur_ref_buff+= cur_key_part->store_length;
  }
  *ref_key= NULL; /* End marker. */
  key_err= 1;
  key_parts= tmp_key_parts;
  DBUG_RETURN(FALSE);
}