  COND *build_equal_items(THD *thd, COND_EQUAL *inherited,
                          bool link_item_fields,
                          COND_EQUAL **cond_equal_ref)
  {
    /*
      normilize_cond() replaced all conditions of type
         WHERE/HAVING field
      to:
        WHERE/HAVING field<>0
      By the time of a build_equal_items() call, all such conditions should
      already be replaced. No Item_field are possible.
      Note, some Item_field derivants are still possible.
      Item_insert_value:
        SELECT * FROM t1 WHERE VALUES(a);
      Item_default_value:
        SELECT * FROM t1 WHERE DEFAULT(a);
    */
    DBUG_ASSERT(type() != FIELD_ITEM);
    return Item_ident::build_equal_items(thd, inherited, link_item_fields,
                                         cond_equal_ref);
  }