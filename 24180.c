int ECDSA_size(const EC_KEY *r)
{
    int ret, i;
    ASN1_INTEGER bs;
    unsigned char buf[4];
    const EC_GROUP *group;

    if (r == NULL)
        return 0;
    group = EC_KEY_get0_group(r);
    if (group == NULL)
        return 0;

    i = EC_GROUP_order_bits(group);
    if (i == 0)
        return 0;
    bs.length = (i + 7) / 8;
    bs.data = buf;
    bs.type = V_ASN1_INTEGER;
    /* If the top bit is set the asn1 encoding is 1 larger. */
    buf[0] = 0xff;

    i = i2d_ASN1_INTEGER(&bs, NULL);
    i += i;                     /* r and s */
    ret = ASN1_object_size(1, i, V_ASN1_SEQUENCE);
    if (ret < 0)
        return 0;
    return ret;
}