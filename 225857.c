void Utf8DecoderBase::WriteUtf16Slow(const uint8_t* stream,
                                     unsigned stream_length,
                                     uint16_t* data,
                                     unsigned data_length) {
  while (data_length != 0) {
    unsigned cursor = 0;

    uint32_t character = Utf8::ValueOf(stream, stream_length, &cursor);
    // There's a total lack of bounds checking for stream
    // as it was already done in Reset.
    stream += cursor;
    stream_length -= cursor;
    if (character > unibrow::Utf16::kMaxNonSurrogateCharCode) {
      *data++ = Utf16::LeadSurrogate(character);
      *data++ = Utf16::TrailSurrogate(character);
      DCHECK(data_length > 1);
      data_length -= 2;
    } else {
      *data++ = character;
      data_length -= 1;
    }
  }
  DCHECK(stream_length >= 0);
}