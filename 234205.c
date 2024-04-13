int RECORD_LAYER_is_sslv2_record(RECORD_LAYER *rl)
{
    return SSL3_RECORD_is_sslv2_record(&rl->rrec[0]);
}