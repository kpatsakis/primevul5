void Lex_input_stream::add_digest_token(uint token, LEX_YYSTYPE yylval)
{
  if (m_digest != NULL)
  {
    m_digest= digest_add_token(m_digest, token, yylval);
  }
}