Pl_AES_PDF::Pl_AES_PDF(char const* identifier, Pipeline* next,
		       bool encrypt, unsigned char const* key,
                       size_t key_bytes) :
    Pipeline(identifier, next),
    crypto(QPDFCryptoProvider::getImpl()),
    encrypt(encrypt),
    cbc_mode(true),
    first(true),
    offset(0),
    key_bytes(key_bytes),
    use_zero_iv(false),
    use_specified_iv(false),
    disable_padding(false)
{
    this->key = std::unique_ptr<unsigned char[]>(
        new unsigned char[key_bytes],
        std::default_delete<unsigned char[]>());
    std::memcpy(this->key.get(), key, key_bytes);
    std::memset(this->inbuf, 0, this->buf_size);
    std::memset(this->outbuf, 0, this->buf_size);
    std::memset(this->cbc_block, 0, this->buf_size);
}