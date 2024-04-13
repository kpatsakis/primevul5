std::string RGWFormPost::get_current_filename() const
{
  try {
    const auto& field = current_data_part->fields.at("Content-Disposition");
    const auto iter = field.params.find("filename");

    if (std::end(field.params) != iter) {
      return prefix + iter->second;
    }
  } catch (std::out_of_range&) {
    /* NOP */;
  }

  return prefix;
}