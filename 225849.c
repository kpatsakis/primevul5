Utf8DecoderBase::Utf8DecoderBase()
  : unbuffered_start_(NULL),
    unbuffered_length_(0),
    utf16_length_(0),
    last_byte_of_buffer_unused_(false) {}