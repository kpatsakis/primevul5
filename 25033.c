bool fix_escape_item(THD *thd, Item *escape_item, String *tmp_str,
                     bool escape_used_in_parsing, CHARSET_INFO *cmp_cs,
                     int *escape)
{
  /*
    ESCAPE clause accepts only constant arguments and Item_param.

    Subqueries during context_analysis_only might decide they're
    const_during_execution, but not quite const yet, not evaluate-able.
    This is fine, as most of context_analysis_only modes will never
    reach val_int(), so we won't need the value.
    CONTEXT_ANALYSIS_ONLY_DERIVED being a notable exception here.
  */
  if (!escape_item->const_during_execution() ||
     (!escape_item->const_item() &&
      !(thd->lex->context_analysis_only & ~CONTEXT_ANALYSIS_ONLY_DERIVED)))
  {
    my_error(ER_WRONG_ARGUMENTS,MYF(0),"ESCAPE");
    return TRUE;
  }

  IF_DBUG(*escape= ESCAPE_NOT_INITIALIZED,);

  if (escape_item->const_item())
  {
    /* If we are on execution stage */
    /* XXX is it safe to evaluate is_expensive() items here? */
    String *escape_str= escape_item->val_str(tmp_str);
    if (escape_str)
    {
      const char *escape_str_ptr= escape_str->ptr();
      if (escape_used_in_parsing && (
             (((thd->variables.sql_mode & MODE_NO_BACKSLASH_ESCAPES) &&
                escape_str->numchars() != 1) ||
               escape_str->numchars() > 1)))
      {
        my_error(ER_WRONG_ARGUMENTS,MYF(0),"ESCAPE");
        return TRUE;
      }

      if (use_mb(cmp_cs))
      {
        CHARSET_INFO *cs= escape_str->charset();
        my_wc_t wc;
        int rc= cs->cset->mb_wc(cs, &wc,
                                (const uchar*) escape_str_ptr,
                                (const uchar*) escape_str_ptr +
                                escape_str->length());
        *escape= (int) (rc > 0 ? wc : '\\');
      }
      else
      {
        /*
          In the case of 8bit character set, we pass native
          code instead of Unicode code as "escape" argument.
          Convert to "cs" if charset of escape differs.
        */
        uint32 unused;
        if (escape_str->needs_conversion(escape_str->length(),
                                         escape_str->charset(),cmp_cs,&unused))
        {
          char ch;
          uint errors;
          uint32 cnvlen= copy_and_convert(&ch, 1, cmp_cs, escape_str_ptr,
                                          escape_str->length(),
                                          escape_str->charset(), &errors);
          *escape= cnvlen ? ch : '\\';
        }
        else
          *escape= escape_str_ptr ? *escape_str_ptr : '\\';
      }
    }
    else
      *escape= '\\';
  }

  return FALSE;
}