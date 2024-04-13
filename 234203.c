void ssl3_record_sequence_update(unsigned char *seq)
{
    int i;

    for (i = 7; i >= 0; i--) {
        ++seq[i];
        if (seq[i] != 0)
            break;
    }
}