int bcf_enc_vfloat(kstring_t *s, int n, float *a)
{
    assert(n >= 0);
    bcf_enc_size(s, n, BCF_BT_FLOAT);
    serialize_float_array(s, n, a);
    return 0; // FIXME: check for errs in this function
}