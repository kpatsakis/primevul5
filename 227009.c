static const char *imap_get_field(enum ConnAccountField field, void *gf_data)
{
  switch (field)
  {
    case MUTT_CA_LOGIN:
      return cs_subset_string(NeoMutt->sub, "imap_login");
    case MUTT_CA_USER:
      return cs_subset_string(NeoMutt->sub, "imap_user");
    case MUTT_CA_PASS:
      return cs_subset_string(NeoMutt->sub, "imap_pass");
    case MUTT_CA_OAUTH_CMD:
      return cs_subset_string(NeoMutt->sub, "imap_oauth_refresh_command");
    case MUTT_CA_HOST:
    default:
      return NULL;
  }
}