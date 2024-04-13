tiffReadCallback(thandle_t  handle,
                 tdata_t    data,
                 tsize_t    length)
{
L_MEMSTREAM  *mstream;
size_t        amount;

    mstream = (L_MEMSTREAM *)handle;
    amount = L_MIN((size_t)length, mstream->hw - mstream->offset);

        /* Fuzzed files can create this condition! */
    if (mstream->offset + amount < amount ||  /* overflow */
        mstream->offset + amount > mstream->hw) {
        lept_stderr("Bad file: amount too big: %zu\n", amount);
        return 0;
    }

    memcpy(data, mstream->buffer + mstream->offset, amount);
    mstream->offset += amount;
    return amount;
}