int canonical_checksum(int csum_type)
{
    return csum_type >= CSUM_MD4 ? 1 : 0;
}
