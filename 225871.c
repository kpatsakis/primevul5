Utf8DecoderBase::Utf8DecoderBase(uint16_t* buffer,
                                 unsigned buffer_length,
                                 const uint8_t* stream,
                                 unsigned stream_length) {
  Reset(buffer, buffer_length, stream, stream_length);
}