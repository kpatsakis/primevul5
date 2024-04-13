String Latin2Decoder::to_utf8(const StringView& input)
{
    StringBuilder builder(input.length());
    for (auto c : input) {
        builder.append_code_point(convert_latin2_to_utf8(c));
    }

    return builder.to_string();
}