bool Item_func_like::fix_fields(THD *thd, Item **ref)
{
  DBUG_ASSERT(fixed == 0);
  if (Item_bool_func2::fix_fields(thd, ref) ||
      escape_item->fix_fields_if_needed_for_scalar(thd, &escape_item) ||
      fix_escape_item(thd, escape_item, &cmp_value1, escape_used_in_parsing,
                      cmp_collation.collation, &escape))
    return TRUE;

  if (escape_item->const_item())
  {
    /*
      We could also do boyer-more for non-const items, but as we would have to
      recompute the tables for each row it's not worth it.
    */
    if (args[1]->const_item() && !use_strnxfrm(collation.collation) &&
        !args[1]->is_expensive())
    {
      String* res2= args[1]->val_str(&cmp_value2);
      if (!res2)
        return FALSE;				// Null argument
      
      const size_t len= res2->length();

      /*
        len must be > 2 ('%pattern%')
        heuristic: only do TurboBM for pattern_len > 2
      */
      if (len <= 2)
        return FALSE;

      const char*  first= res2->ptr();
      const char*  last=  first + len - 1;
      
      if (len > MIN_TURBOBM_PATTERN_LEN + 2 &&
          *first == wild_many &&
          *last  == wild_many)
      {
        const char* tmp = first + 1;
        for (; *tmp != wild_many && *tmp != wild_one && *tmp != escape; tmp++) ;
        canDoTurboBM = (tmp == last) && !use_mb(args[0]->collation.collation);
      }
      if (canDoTurboBM)
      {
        pattern_len = (int) len - 2;
        pattern     = thd->strmake(first + 1, pattern_len);
        DBUG_PRINT("info", ("Initializing pattern: '%s'", first));
        int *suff = (int*) thd->alloc((int) (sizeof(int)*
                                      ((pattern_len + 1)*2+
                                      alphabet_size)));
        bmGs      = suff + pattern_len + 1;
        bmBc      = bmGs + pattern_len + 1;
        turboBM_compute_good_suffix_shifts(suff);
        turboBM_compute_bad_character_shifts();
        DBUG_PRINT("info",("done"));
      }
      use_sampling= (len > 2 && (*first == wild_many || *first == wild_one));
    }
  }
  return FALSE;
}