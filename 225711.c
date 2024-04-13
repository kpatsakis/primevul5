mptcp_cryptodata_sha1(const guint64 key, guint32 *token, guint64 *idsn)
{
    guint8 digest_buf[HASH_SHA1_LENGTH];
    guint64 pseudokey = GUINT64_TO_BE(key);
    guint32 _token;
    guint64 _isdn;

    gcry_md_hash_buffer(GCRY_MD_SHA1, digest_buf, (const guint8 *)&pseudokey, 8);

    /* memcpy to prevent -Wstrict-aliasing errors with GCC 4 */
    memcpy(&_token, digest_buf, sizeof(_token));
    *token = GUINT32_FROM_BE(_token);
    memcpy(&_isdn, digest_buf + HASH_SHA1_LENGTH - sizeof(_isdn), sizeof(_isdn));
    *idsn = GUINT64_FROM_BE(_isdn);
}