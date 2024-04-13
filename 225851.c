Utf8Decoder<kBufferSize>::Utf8Decoder(const char* stream, unsigned length)
  : Utf8DecoderBase(buffer_,
                    kBufferSize,
                    reinterpret_cast<const uint8_t*>(stream),
                    length) {
}