UnsignedBigInteger random_big_prime(size_t bits)
{
    ASSERT(bits >= 33);
    UnsignedBigInteger min = UnsignedBigInteger::from_base10("6074001000").shift_left(bits - 33);
    UnsignedBigInteger max = UnsignedBigInteger { 1 }.shift_left(bits).minus(1);
    for (;;) {
        auto p = random_number(min, max);
        if ((p.words()[0] & 1) == 0) {
            // An even number is definitely not a large prime.
            continue;
        }
        if (is_probably_prime(p))
            return p;
    }
}