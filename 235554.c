int PasswdMgr::encryptDecryptData(bool doEncrypt, const EVP_CIPHER* cipher,
                                  uint8_t* key, size_t keyLen, uint8_t* iv,
                                  size_t ivLen, uint8_t* inBytes,
                                  size_t inBytesLen, uint8_t* mac,
                                  size_t* macLen, unsigned char* outBytes,
                                  size_t* outBytesLen)
{
    if (cipher == NULL || key == NULL || iv == NULL || inBytes == NULL ||
        outBytes == NULL || mac == NULL || inBytesLen == 0 ||
        (size_t)EVP_CIPHER_key_length(cipher) > keyLen ||
        (size_t)EVP_CIPHER_iv_length(cipher) > ivLen)
    {
        log<level::DEBUG>("Error Invalid Inputs");
        return -EINVAL;
    }

    if (!doEncrypt)
    {
        // verify MAC before decrypting the data.
        std::array<uint8_t, EVP_MAX_MD_SIZE> calMac;
        size_t calMacLen = calMac.size();
        // calculate MAC for the encrypted message.
        if (NULL == HMAC(EVP_sha256(), key, keyLen, inBytes, inBytesLen,
                         calMac.data(),
                         reinterpret_cast<unsigned int*>(&calMacLen)))
        {
            log<level::DEBUG>("Error: Failed to calculate MAC");
            return -EIO;
        }
        if (!((calMacLen == *macLen) &&
              (std::memcmp(calMac.data(), mac, calMacLen) == 0)))
        {
            log<level::DEBUG>("Authenticated message doesn't match");
            return -EBADMSG;
        }
    }

    std::unique_ptr<EVP_CIPHER_CTX, decltype(&::EVP_CIPHER_CTX_free)> ctx(
        EVP_CIPHER_CTX_new(), ::EVP_CIPHER_CTX_free);
    EVP_CIPHER_CTX_set_padding(ctx.get(), 1);

    // Set key & IV
    int retval = EVP_CipherInit_ex(ctx.get(), cipher, NULL, key, iv,
                                   static_cast<int>(doEncrypt));
    if (!retval)
    {
        log<level::DEBUG>("EVP_CipherInit_ex failed",
                          entry("RET_VAL=%d", retval));
        return -EIO;
    }

    int outLen = 0, outEVPLen = 0;
    if ((retval = EVP_CipherUpdate(ctx.get(), outBytes + outLen, &outEVPLen,
                                   inBytes, inBytesLen)))
    {
        outLen += outEVPLen;
        if ((retval =
                 EVP_CipherFinal(ctx.get(), outBytes + outLen, &outEVPLen)))
        {
            outLen += outEVPLen;
            *outBytesLen = outLen;
        }
        else
        {
            log<level::DEBUG>("EVP_CipherFinal fails",
                              entry("RET_VAL=%d", retval));
            return -EIO;
        }
    }
    else
    {
        log<level::DEBUG>("EVP_CipherUpdate fails",
                          entry("RET_VAL=%d", retval));
        return -EIO;
    }

    if (doEncrypt)
    {
        // Create MAC for the encrypted message
        if (NULL == HMAC(EVP_sha256(), key, keyLen, outBytes, *outBytesLen, mac,
                         reinterpret_cast<unsigned int*>(macLen)))
        {
            log<level::DEBUG>("Failed to create authentication");
            return -EIO;
        }
    }
    return 0;
}