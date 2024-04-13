static int get_swift_versioning_settings(
  req_state * const s,
  boost::optional<std::string>& swift_ver_location)
{
  /* Removing the Swift's versions location has lower priority than setting
   * a new one. That's the reason why we're handling it first. */
  const std::string vlocdel =
    s->info.env->get("HTTP_X_REMOVE_VERSIONS_LOCATION", "");
  if (vlocdel.size()) {
    swift_ver_location = boost::in_place(std::string());
  }

  if (s->info.env->exists("HTTP_X_VERSIONS_LOCATION")) {
    /* If the Swift's versioning is globally disabled but someone wants to
     * enable it for a given container, new version of Swift will generate
     * the precondition failed error. */
    if (! s->cct->_conf->rgw_swift_versioning_enabled) {
      return -ERR_PRECONDITION_FAILED;
    }

    swift_ver_location = s->info.env->get("HTTP_X_VERSIONS_LOCATION", "");
  }

  return 0;
}