bool Lex_input_stream::get_text(LEX_STRING *dst, uint sep,
                                int pre_skip, int post_skip)
{
  uchar c;
  uint found_escape=0;
  CHARSET_INFO *cs= m_thd->charset();

  tok_bitmap= 0;
  while (! eof())
  {
    c= yyGet();
    tok_bitmap|= c;
#ifdef USE_MB
    {
      int l;
      if (use_mb(cs) &&
          (l = my_ismbchar(cs,
                           get_ptr() -1,
                           get_end_of_query()))) {
        skip_binary(l-1);
        continue;
      }
    }
#endif
    if (c == '\\' &&
        !(m_thd->variables.sql_mode & MODE_NO_BACKSLASH_ESCAPES))
    {					// Escaped character
      found_escape=1;
      if (eof())
	return true;
      yySkip();
    }
    else if (c == sep)
    {
      if (c == yyGet())                 // Check if two separators in a row
      {
        found_escape=1;                 // duplicate. Remember for delete
	continue;
      }
      else
        yyUnget();

      /* Found end. Unescape and return string */
      const char *str, *end;

      str= get_tok_start();
      end= get_ptr();
      /* Extract the text from the token */
      str += pre_skip;
      end -= post_skip;
      DBUG_ASSERT(end >= str);

      if (!(dst->str= (char*) m_thd->alloc((uint) (end - str) + 1)))
      {
        dst->str= (char*) "";        // Sql_alloc has set error flag
        dst->length= 0;
        return true;
      }

      m_cpp_text_start= get_cpp_tok_start() + pre_skip;
      m_cpp_text_end= get_cpp_ptr() - post_skip;

      if (!found_escape)
      {
        memcpy(dst->str, str, dst->length= (end - str));
        dst->str[dst->length]= 0;
      }
      else
      {
        dst->length= unescape(cs, dst->str, str, end, sep);
      }
      return false;
    }
  }
  return true;                         // unexpected end of query
}