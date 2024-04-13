bool consume_comment(Lex_input_stream *lip, int remaining_recursions_permitted)
{
  // only one level of nested comments are allowed
  DBUG_ASSERT(remaining_recursions_permitted == 0 ||
              remaining_recursions_permitted == 1);
  uchar c;
  while (! lip->eof())
  {
    c= lip->yyGet();

    if (remaining_recursions_permitted == 1)
    {
      if ((c == '/') && (lip->yyPeek() == '*'))
      {
        lip->yyUnput('(');  // Replace nested "/*..." with "(*..."
        lip->yySkip();      // and skip "("

        lip->yySkip(); /* Eat asterisk */
        if (consume_comment(lip, 0))
          return true;

        lip->yyUnput(')');  // Replace "...*/" with "...*)"
        lip->yySkip();      // and skip ")"
        continue;
      }
    }

    if (c == '*')
    {
      if (lip->yyPeek() == '/')
      {
        lip->yySkip(); /* Eat slash */
        return FALSE;
      }
    }

    if (c == '\n')
      lip->yylineno++;
  }

  return TRUE;
}