int RGWHandler_REST_SWIFT::init_from_header(struct req_state* const s,
                                            const std::string& frontend_prefix)
{
  string req;
  string first;

  s->prot_flags |= RGW_REST_SWIFT;

  char reqbuf[frontend_prefix.length() + s->decoded_uri.length() + 1];
  sprintf(reqbuf, "%s%s", frontend_prefix.c_str(), s->decoded_uri.c_str());
  const char *req_name = reqbuf;

  const char *p;

  if (*req_name == '?') {
    p = req_name;
  } else {
    p = s->info.request_params.c_str();
  }

  s->info.args.set(p);
  s->info.args.parse();

  /* Skip the leading slash of URL hierarchy. */
  if (req_name[0] != '/') {
    return 0;
  } else {
    req_name++;
  }

  if ('\0' == req_name[0]) {
    return g_conf()->rgw_swift_url_prefix == "/" ? -ERR_BAD_URL : 0;
  }

  req = req_name;

  size_t pos = req.find('/');
  if (std::string::npos != pos && g_conf()->rgw_swift_url_prefix != "/") {
    bool cut_url = g_conf()->rgw_swift_url_prefix.length();
    first = req.substr(0, pos);

    if (first.compare(g_conf()->rgw_swift_url_prefix) == 0) {
      if (cut_url) {
        /* Rewind to the "v1/..." part. */
        next_tok(req, first, '/');
      }
    }
  } else if (req.compare(g_conf()->rgw_swift_url_prefix) == 0) {
    s->formatter = new RGWFormatter_Plain;
    return -ERR_BAD_URL;
  } else {
    first = req;
  }

  std::string tenant_path;
  if (! g_conf()->rgw_swift_tenant_name.empty()) {
    tenant_path = "/AUTH_";
    tenant_path.append(g_conf()->rgw_swift_tenant_name);
  }

  /* verify that the request_uri conforms with what's expected */
  char buf[g_conf()->rgw_swift_url_prefix.length() + 16 + tenant_path.length()];
  int blen;
  if (g_conf()->rgw_swift_url_prefix == "/") {
    blen = sprintf(buf, "/v1%s", tenant_path.c_str());
  } else {
    blen = sprintf(buf, "/%s/v1%s",
                   g_conf()->rgw_swift_url_prefix.c_str(), tenant_path.c_str());
  }

  if (strncmp(reqbuf, buf, blen) != 0) {
    return -ENOENT;
  }

  int ret = allocate_formatter(s, RGW_FORMAT_PLAIN, true);
  if (ret < 0)
    return ret;

  string ver;

  next_tok(req, ver, '/');

  if (!tenant_path.empty() || g_conf()->rgw_swift_account_in_url) {
    string account_name;
    next_tok(req, account_name, '/');

    /* Erase all pre-defined prefixes like "AUTH_" or "KEY_". */
    const vector<string> skipped_prefixes = { "AUTH_", "KEY_" };

    for (const auto pfx : skipped_prefixes) {
      const size_t comp_len = min(account_name.length(), pfx.length());
      if (account_name.compare(0, comp_len, pfx) == 0) {
        /* Prefix is present. Drop it. */
        account_name = account_name.substr(comp_len);
        break;
      }
    }

    if (account_name.empty()) {
      return -ERR_PRECONDITION_FAILED;
    } else {
      s->account_name = account_name;
    }
  }

  next_tok(req, first, '/');

  dout(10) << "ver=" << ver << " first=" << first << " req=" << req << dendl;
  if (first.size() == 0)
    return 0;

  s->info.effective_uri = "/" + first;

  // Save bucket to tide us over until token is parsed.
  s->init_state.url_bucket = first;

  if (req.size()) {
    s->object =
      rgw_obj_key(req, s->info.env->get("HTTP_X_OBJECT_VERSION_ID", "")); /* rgw swift extension */
    s->info.effective_uri.append("/" + s->object.name);
  }

  return 0;
}