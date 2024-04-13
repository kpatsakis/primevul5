void evp(int nid, unsigned char *digest, int len, unsigned char *hash, unsigned int *hlen)
{
    const EVP_MD *evp_md = nid_to_evpmd(nid);
    EVP_MD_CTX md;

    EVP_DigestInit(&md, evp_md);
    EVP_DigestUpdate(&md, digest, len);
    EVP_DigestFinal(&md, hash, hlen);
}