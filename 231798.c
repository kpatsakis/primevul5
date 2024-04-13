bool RGWFormPost::is_next_file_to_upload()
{
  if (! stream_done) {
    /* We have at least one additional part in the body. */
    struct post_form_part part;
    int r = read_form_part_header(&part, stream_done);
    if (r < 0) {
      return false;
    }

    const auto field_iter = part.fields.find("Content-Disposition");
    if (std::end(part.fields) != field_iter) {
      const auto& params = field_iter->second.params;
      const auto& filename_iter = params.find("filename");

      if (std::end(params) != filename_iter && ! filename_iter->second.empty()) {
        current_data_part = std::move(part);
        return true;
      }
    }
  }

  return false;
}