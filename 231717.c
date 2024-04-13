int RGWFormPost::get_params()
{
  /* The parentt class extracts boundary info from the Content-Type. */
  int ret = RGWPostObj_ObjStore::get_params();
  if (ret < 0) {
    return ret;
  }

  policy.create_default(s->user->user_id, s->user->display_name);

  /* Let's start parsing the HTTP body by parsing each form part step-
   * by-step till encountering the first part with file data. */
  do {
    struct post_form_part part;
    ret = read_form_part_header(&part, stream_done);
    if (ret < 0) {
      return ret;
    }

    if (s->cct->_conf->subsys.should_gather<ceph_subsys_rgw, 20>()) {
      ldout(s->cct, 20) << "read part header -- part.name="
                        << part.name << dendl;

      for (const auto& pair : part.fields) {
        ldout(s->cct, 20) << "field.name=" << pair.first << dendl;
        ldout(s->cct, 20) << "field.val=" << pair.second.val << dendl;
        ldout(s->cct, 20) << "field.params:" << dendl;

        for (const auto& param_pair : pair.second.params) {
          ldout(s->cct, 20) << " " << param_pair.first
                            << " -> " << param_pair.second << dendl;
        }
      }
    }

    if (stream_done) {
      /* Unexpected here. */
      err_msg = "Malformed request";
      return -EINVAL;
    }

    const auto field_iter = part.fields.find("Content-Disposition");
    if (std::end(part.fields) != field_iter &&
        std::end(field_iter->second.params) != field_iter->second.params.find("filename")) {
      /* First data part ahead. */
      current_data_part = std::move(part);

      /* Stop the iteration. We can assume that all control parts have been
       * already parsed. The rest of HTTP body should contain data parts
       * only. They will be picked up by ::get_data(). */
      break;
    } else {
      /* Control part ahead. Receive, parse and store for later usage. */
      bool boundary;
      ret = read_data(part.data, s->cct->_conf->rgw_max_chunk_size,
                      boundary, stream_done);
      if (ret < 0) {
        return ret;
      } else if (! boundary) {
        err_msg = "Couldn't find boundary";
        return -EINVAL;
      }

      ctrl_parts[part.name] = std::move(part);
    }
  } while (! stream_done);

  min_len = 0;
  max_len = get_max_file_size();

  if (! current_data_part) {
    err_msg = "FormPost: no files to process";
    return -EINVAL;
  }

  if (! is_non_expired()) {
    err_msg = "FormPost: Form Expired";
    return -EPERM;
  }

  if (! is_integral()) {
    err_msg = "FormPost: Invalid Signature";
    return -EPERM;
  }

  return 0;
}