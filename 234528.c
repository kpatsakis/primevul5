add_key_part(DYNAMIC_ARRAY *keyuse_array, KEY_FIELD *key_field)
{
  Field *field=key_field->field;
  TABLE *form= field->table;

  if (key_field->eq_func && !(key_field->optimize & KEY_OPTIMIZE_EXISTS))
  {
    for (uint key=0 ; key < form->s->keys ; key++)
    {
      if (!(form->keys_in_use_for_query.is_set(key)))
	continue;
      if (form->key_info[key].flags & (HA_FULLTEXT | HA_SPATIAL))
	continue;    // ToDo: ft-keys in non-ft queries.   SerG

      KEY *keyinfo= form->key_info+key;
      uint key_parts= form->actual_n_key_parts(keyinfo);
      for (uint part=0 ; part <  key_parts ; part++)
      {
        if (field->eq(form->key_info[key].key_part[part].field) &&
            field->can_optimize_keypart_ref(key_field->cond, key_field->val))
	{
          if (add_keyuse(keyuse_array, key_field, key, part))
            return TRUE;
	}
      }
    }
    if (field->hash_join_is_possible() &&
        (key_field->optimize & KEY_OPTIMIZE_EQ) &&
        key_field->val->used_tables())
    {
      if (!field->can_optimize_hash_join(key_field->cond, key_field->val))
        return false;
      if (form->is_splittable())
        form->add_splitting_info_for_key_field(key_field);
      /* 
        If a key use is extracted from an equi-join predicate then it is
        added not only as a key use for every index whose component can
        be evalusted utilizing this key use, but also as a key use for
        hash join. Such key uses are marked with a special key number. 
      */    
      if (add_keyuse(keyuse_array, key_field, get_hash_join_key_no(), 0))
        return TRUE;
    }
  }
  return FALSE;
}