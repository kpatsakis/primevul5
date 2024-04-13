static bool create_hj_key_for_table(JOIN *join, JOIN_TAB *join_tab,
                                    KEYUSE *org_keyuse, table_map used_tables)
{
  KEY *keyinfo;
  KEY_PART_INFO *key_part_info;
  KEYUSE *keyuse= org_keyuse;
  uint key_parts= 0;
  THD  *thd= join->thd;
  TABLE *table= join_tab->table;
  bool first_keyuse= TRUE;
  DBUG_ENTER("create_hj_key_for_table");

  do
  {
    if (!(~used_tables & keyuse->used_tables) &&
        join_tab->keyuse_is_valid_for_access_in_chosen_plan(join, keyuse) &&
        are_tables_local(join_tab, keyuse->used_tables))    
    {
      if (first_keyuse)
      {
        key_parts++;
      }
      else
      {
        KEYUSE *curr= org_keyuse;
        for( ; curr < keyuse; curr++)
        {
          if (curr->keypart == keyuse->keypart &&
              !(~used_tables & curr->used_tables) &&
              join_tab->keyuse_is_valid_for_access_in_chosen_plan(join,
                                                                  curr) &&
              are_tables_local(join_tab, curr->used_tables))
            break;
        }
        if (curr == keyuse)
           key_parts++;
      }
    }
    first_keyuse= FALSE;
    keyuse++;
  } while (keyuse->table == table && keyuse->is_for_hash_join());
  if (!key_parts)
    DBUG_RETURN(TRUE);
  /* This memory is allocated only once for the joined table join_tab */
  if (!(keyinfo= (KEY *) thd->alloc(sizeof(KEY))) ||
      !(key_part_info = (KEY_PART_INFO *) thd->alloc(sizeof(KEY_PART_INFO)*
                                                     key_parts)))
    DBUG_RETURN(TRUE);
  keyinfo->usable_key_parts= keyinfo->user_defined_key_parts = key_parts;
  keyinfo->ext_key_parts= keyinfo->user_defined_key_parts;
  keyinfo->key_part= key_part_info;
  keyinfo->key_length=0;
  keyinfo->algorithm= HA_KEY_ALG_UNDEF;
  keyinfo->flags= HA_GENERATED_KEY;
  keyinfo->is_statistics_from_stat_tables= FALSE;
  keyinfo->name= (char *) "$hj";
  keyinfo->rec_per_key= (ulong*) thd->calloc(sizeof(ulong)*key_parts);
  if (!keyinfo->rec_per_key)
    DBUG_RETURN(TRUE);
  keyinfo->key_part= key_part_info;

  first_keyuse= TRUE;
  keyuse= org_keyuse;
  do
  {
    if (!(~used_tables & keyuse->used_tables) &&
        join_tab->keyuse_is_valid_for_access_in_chosen_plan(join, keyuse) &&
        are_tables_local(join_tab, keyuse->used_tables))
    { 
      bool add_key_part= TRUE;
      if (!first_keyuse)
      {
        for(KEYUSE *curr= org_keyuse; curr < keyuse; curr++)
        {
          if (curr->keypart == keyuse->keypart &&
              !(~used_tables & curr->used_tables) &&
              join_tab->keyuse_is_valid_for_access_in_chosen_plan(join,
                                                                  curr) &&
              are_tables_local(join_tab, curr->used_tables))
	  {
            keyuse->keypart= NO_KEYPART;
            add_key_part= FALSE;
            break;
          }
        }
      }
      if (add_key_part)
      {
        Field *field= table->field[keyuse->keypart];
        uint fieldnr= keyuse->keypart+1;
        table->create_key_part_by_field(key_part_info, field, fieldnr);
        keyinfo->key_length += key_part_info->store_length;
        key_part_info++;
      }
    }
    first_keyuse= FALSE;
    keyuse++;
  } while (keyuse->table == table && keyuse->is_for_hash_join());

  keyinfo->ext_key_parts= keyinfo->user_defined_key_parts;
  keyinfo->ext_key_flags= keyinfo->flags;
  keyinfo->ext_key_part_map= 0;

  join_tab->hj_key= keyinfo;

  DBUG_RETURN(FALSE);
}