bool is_standardized_encoding(const String& encoding)
{
    return encoding.equals_ignoring_case(get_standardized_encoding(encoding));
}