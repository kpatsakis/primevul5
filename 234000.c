scheme_disable (enum url_scheme scheme)
{
  supported_schemes[scheme].flags |= scm_disabled;
}