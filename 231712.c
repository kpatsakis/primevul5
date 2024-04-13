std::string RGWFormPost::get_current_content_type() const
{
  try {
    const auto& field = current_data_part->fields.at("Content-Type");
    return field.val;
  } catch (std::out_of_range&) {
    /* NOP */;
  }

  return std::string();
}