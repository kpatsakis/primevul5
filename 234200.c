unsigned int RECORD_LAYER_get_rrec_length(RECORD_LAYER *rl)
{
    return SSL3_RECORD_get_length(&rl->rrec[0]);
}