static const EVP_MD *nid_to_evpmd(int nid)
{
    switch (nid) {
        case NID_X9_62_prime256v1:
            return EVP_sha256();
        case NID_secp384r1:
            return EVP_sha384();
        case NID_secp521r1:
            return EVP_sha512();
        default:
            return NULL;
    }

    return NULL;
}