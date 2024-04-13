CLOSE_BRACKET_WITHOUT_ESC_WARN(ScanEnv* env, UChar* c)
{
  if (onig_warn == onig_null_warn) return ;

  if (IS_SYNTAX_BV((env)->syntax, ONIG_SYN_WARN_CC_OP_NOT_ESCAPED)) {
      onig_syntax_warn(env, "regular expression has '%s' without escape", c);
  }
}