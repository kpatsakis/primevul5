uint get_next_field_for_derived_key(uchar *arg)
{
  KEYUSE *keyuse= *(KEYUSE **) arg;
  if (!keyuse)
    return (uint) (-1);
  TABLE *table= keyuse->table;
  uint key= keyuse->key;
  uint fldno= keyuse->keypart; 
  uint keypart= keyuse->keypart_map == (key_part_map) 1 ?
                                         0 : (keyuse-1)->keypart+1;
  for ( ; 
        keyuse->table == table && keyuse->key == key && keyuse->keypart == fldno;
        keyuse++)
    keyuse->keypart= keypart;
  if (keyuse->key != key)
    keyuse= 0;
  *((KEYUSE **) arg)= keyuse;
  return fldno;
}