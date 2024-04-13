FastHufDecoder::buildTables (Int64 *base, Int64 *offset)
{
    //
    // Build the 'left justified' base table, by shifting base left..
    //

    for (int i = 0; i <= MAX_CODE_LEN; ++i)
    {
        if (base[i] != 0xffffffffffffffffULL)
        {
            _ljBase[i] = base[i] << (64 - i);
        }
        else
        {
            //
            // Unused code length - insert dummy values
            //

            _ljBase[i] = 0xffffffffffffffffULL;
        }
    }

    //
    // Combine some terms into a big fat constant, which for
    // lack of a better term we'll call the 'left justified' 
    // offset table (because it serves the same function
    // as 'offset', when using the left justified base table.
    //

    _ljOffset[0] = offset[0] - _ljBase[0];
    for (int i = 1; i <= MAX_CODE_LEN; ++i)
        _ljOffset[i] = offset[i] - (_ljBase[i] >> (64 - i));

    //
    // Build the acceleration tables for the lookups of
    // short codes ( <= TABLE_LOOKUP_BITS long)
    //

    for (Int64 i = 0; i < 1 << TABLE_LOOKUP_BITS; ++i)
    {
        Int64 value = i << (64 - TABLE_LOOKUP_BITS);

        _tableSymbol[i]  = 0xffff;
        _tableCodeLen[i] = 0; 

        for (int codeLen = _minCodeLength; codeLen <= _maxCodeLength; ++codeLen)
        {
            if (_ljBase[codeLen] <= value)
            {
                _tableCodeLen[i] = codeLen;

                Int64 id = _ljOffset[codeLen] + (value >> (64 - codeLen));
                if (id < static_cast<Int64>(_numSymbols))
                {
                    _tableSymbol[i] = _idToSymbol[id];
                }
                else
                {
                    throw IEX_NAMESPACE::InputExc ("Huffman decode error "
                                                   "(Overrun).");
                }
                break;
            }
        }
    }

    //
    // Store the smallest value in the table that points to real data.
    // This should be the entry for the largest length that has 
    // valid data (in our case, non-dummy _ljBase)
    //

    int minIdx = TABLE_LOOKUP_BITS;

    while (minIdx > 0 && _ljBase[minIdx] == 0xffffffffffffffffULL)
        minIdx--;

    if (minIdx < 0)
    {
        //
        // Error, no codes with lengths 0-TABLE_LOOKUP_BITS used.
        // Set the min value such that the table is never tested.
        //

        _tableMin = 0xffffffffffffffffULL;
    }
    else
    {
        _tableMin = _ljBase[minIdx];
    }
}