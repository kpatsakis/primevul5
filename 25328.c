bool Item_func_case_simple::aggregate_switch_and_when_arguments(THD *thd,
                                                                bool nulls_eq)
{
  uint ncases= when_count();
  m_found_types= 0;
  if (prepare_predicant_and_values(thd, &m_found_types, nulls_eq))
  {
    /*
      If Predicant_to_list_comparator() fails to prepare components,
      it must put an error into the diagnostics area. This is needed
      to make fix_fields() catches such errors.
    */
    DBUG_ASSERT(thd->is_error());
    return true;
  }

  if (!(m_found_types= collect_cmp_types(args, ncases + 1)))
    return true;

  if (m_found_types & (1U << STRING_RESULT))
  {
    /*
      If we'll do string comparison, we also need to aggregate
      character set and collation for first/WHEN items and
      install converters for some of them to cmp_collation when necessary.
      This is done because cmp_item compatators cannot compare
      strings in two different character sets.
      Some examples when we install converters:

      1. Converter installed for the first expression:

         CASE         latin1_item              WHEN utf16_item THEN ... END

      is replaced to:

         CASE CONVERT(latin1_item USING utf16) WHEN utf16_item THEN ... END

      2. Converter installed for the left WHEN item:

        CASE utf16_item WHEN         latin1_item              THEN ... END

      is replaced to:

         CASE utf16_item WHEN CONVERT(latin1_item USING utf16) THEN ... END
    */
    if (agg_arg_charsets_for_comparison(cmp_collation, args, ncases + 1))
      return true;
  }

  if (make_unique_cmp_items(thd, cmp_collation.collation))
    return true;

  return false;
}