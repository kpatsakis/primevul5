add_keyuse(DYNAMIC_ARRAY *keyuse_array, KEY_FIELD *key_field,
          uint key, uint part)
{
  KEYUSE keyuse;
  Field *field= key_field->field;

  keyuse.table= field->table;
  keyuse.val= key_field->val;
  keyuse.key= key;
  if (!is_hash_join_key_no(key))
  {
    keyuse.keypart=part;
    keyuse.keypart_map= (key_part_map) 1 << part;
  }
  else
  {
    keyuse.keypart= field->field_index;
    keyuse.keypart_map= (key_part_map) 0;
  }
  keyuse.used_tables= key_field->val->used_tables();
  keyuse.optimize= key_field->optimize & KEY_OPTIMIZE_REF_OR_NULL;
  keyuse.ref_table_rows= 0;
  keyuse.null_rejecting= key_field->null_rejecting;
  keyuse.cond_guard= key_field->cond_guard;
  keyuse.sj_pred_no= key_field->sj_pred_no;
  keyuse.validity_ref= 0;
  return (insert_dynamic(keyuse_array,(uchar*) &keyuse));
}