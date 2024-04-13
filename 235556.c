int PasswdMgr::readPasswdFileData(std::vector<uint8_t>& outBytes)
{
    std::array<uint8_t, maxKeySize> keyBuff;
    std::ifstream keyFile(encryptKeyFileName, std::ios::in | std::ios::binary);
    if (!keyFile.is_open())
    {
        log<level::DEBUG>("Error in opening encryption key file");
        return -EIO;
    }
    keyFile.read(reinterpret_cast<char*>(keyBuff.data()), keyBuff.size());
    if (keyFile.fail())
    {
        log<level::DEBUG>("Error in reading encryption key file");
        return -EIO;
    }

    std::ifstream passwdFile(passwdFileName, std::ios::in | std::ios::binary);
    if (!passwdFile.is_open())
    {
        log<level::DEBUG>("Error in opening ipmi password file");
        return -EIO;
    }

    // calculate file size and read the data
    passwdFile.seekg(0, std::ios::end);
    ssize_t fileSize = passwdFile.tellg();
    passwdFile.seekg(0, std::ios::beg);
    std::vector<uint8_t> input(fileSize);
    passwdFile.read(reinterpret_cast<char*>(input.data()), fileSize);
    if (passwdFile.fail())
    {
        log<level::DEBUG>("Error in reading encryption key file");
        return -EIO;
    }

    // verify the signature first
    MetaPassStruct* metaData = reinterpret_cast<MetaPassStruct*>(input.data());
    if (std::strncmp(metaData->signature, META_PASSWD_SIG,
                     sizeof(metaData->signature)))
    {
        log<level::DEBUG>("Error signature mismatch in password file");
        return -EBADMSG;
    }

    size_t inBytesLen = metaData->dataSize + metaData->padSize;
    // If data is empty i.e no password map then return success
    if (inBytesLen == 0)
    {
        log<level::DEBUG>("Empty password file");
        return 0;
    }

    // compute the key needed to decrypt
    std::array<uint8_t, EVP_MAX_KEY_LENGTH> key;
    auto keyLen = key.size();
    if (NULL == HMAC(EVP_sha256(), keyBuff.data(), keyBuff.size(),
                     input.data() + sizeof(*metaData), metaData->hashSize,
                     key.data(), reinterpret_cast<unsigned int*>(&keyLen)))
    {
        log<level::DEBUG>("Failed to create MAC for authentication");
        return -EIO;
    }

    // decrypt the data
    uint8_t* iv = input.data() + sizeof(*metaData) + metaData->hashSize;
    size_t ivLen = metaData->ivSize;
    uint8_t* inBytes = iv + ivLen;
    uint8_t* mac = inBytes + inBytesLen;
    size_t macLen = metaData->macSize;

    size_t outBytesLen = 0;
    // Resize to actual data size
    outBytes.resize(inBytesLen + EVP_MAX_BLOCK_LENGTH);
    if (encryptDecryptData(false, EVP_aes_128_cbc(), key.data(), keyLen, iv,
                           ivLen, inBytes, inBytesLen, mac, &macLen,
                           outBytes.data(), &outBytesLen) != 0)
    {
        log<level::DEBUG>("Error in decryption");
        return -EIO;
    }
    // Resize the vector to outBytesLen
    outBytes.resize(outBytesLen);

    OPENSSL_cleanse(key.data(), keyLen);
    OPENSSL_cleanse(iv, ivLen);

    return 0;
}