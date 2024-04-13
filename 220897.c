int RGWDeleteObj_ObjStore_S3::get_params()
{
  const char *if_unmod = s->info.env->get("HTTP_X_AMZ_DELETE_IF_UNMODIFIED_SINCE");

  if (s->system_request) {
    s->info.args.get_bool(RGW_SYS_PARAM_PREFIX "no-precondition-error", &no_precondition_error, false);
  }

  if (if_unmod) {
    std::string if_unmod_decoded = url_decode(if_unmod);
    uint64_t epoch;
    uint64_t nsec;
    if (utime_t::parse_date(if_unmod_decoded, &epoch, &nsec) < 0) {
      ldout(s->cct, 10) << "failed to parse time: " << if_unmod_decoded << dendl;
      return -EINVAL;
    }
    unmod_since = utime_t(epoch, nsec).to_real_time();
  }

  const char *bypass_gov_header = s->info.env->get("HTTP_X_AMZ_BYPASS_GOVERNANCE_RETENTION");
  if (bypass_gov_header) {
    std::string bypass_gov_decoded = url_decode(bypass_gov_header);
    bypass_governance_mode = boost::algorithm::iequals(bypass_gov_decoded, "true");
  }

  return 0;
}