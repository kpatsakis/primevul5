    TestCheckedArrayByteSink(char* outbuf, int32_t capacity)
            : CheckedArrayByteSink(outbuf, capacity), calledFlush(FALSE) {}