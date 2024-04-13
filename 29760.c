int test_capabilty(uint32_t *caps, int num_caps, uint32_t cap)
{
    uint32_t index = cap / 32;
    if (num_caps < index + 1) {
        return FALSE;
    }

    return (caps[index] & (1 << (cap % 32))) != 0;
}
