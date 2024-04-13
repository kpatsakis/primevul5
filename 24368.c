void Lex_input_stream::reduce_digest_token(uint token_left, uint token_right)
{
  if (m_digest != NULL)
  {
    m_digest= digest_reduce_token(m_digest, token_left, token_right);
  }
}