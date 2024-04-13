bool JOIN_TAB::hash_join_is_possible()
{
  if (type != JT_REF && type != JT_EQ_REF)
    return FALSE;
  if (!is_ref_for_hash_join())
  {
    KEY *keyinfo= table->key_info + ref.key;
    return keyinfo->key_part[0].field->hash_join_is_possible();
  }
  return TRUE;
}