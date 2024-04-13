void Utf8Decoder<kBufferSize>::Reset(const char* stream, unsigned length) {
  Utf8DecoderBase::Reset(buffer_,
                         kBufferSize,
                         reinterpret_cast<const uint8_t*>(stream),
                         length);
}