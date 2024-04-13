uint get_next_field_for_derived_key_simple(uchar *arg)
{
  KEYUSE *keyuse= *(KEYUSE **) arg;
  if (!keyuse)
    return (uint) (-1);
  TABLE *table= keyuse->table;
  uint key= keyuse->key;
  uint fldno= keyuse->keypart;
  for ( ;
        keyuse->table == table && keyuse->key == key && keyuse->keypart == fldno;
        keyuse++)
    ;
  if (keyuse->key != key)
    keyuse= 0;
  *((KEYUSE **) arg)= keyuse;
  return fldno;
}