bool is_probably_prime(const UnsignedBigInteger& p)
{
    // Is it a small number?
    if (p < 49) {
        u32 p_value = p.words()[0];
        // Is it a very small prime?
        if (p_value == 2 || p_value == 3 || p_value == 5 || p_value == 7)
            return true;
        // Is it the multiple of a very small prime?
        if (p_value % 2 == 0 || p_value % 3 == 0 || p_value % 5 == 0 || p_value % 7 == 0)
            return false;
        // Then it must be a prime, but not a very small prime, like 37.
        return true;
    }

    Vector<UnsignedBigInteger, 256> tests;
    // Make some good initial guesses that are guaranteed to find all primes < 2^64.
    tests.append(UnsignedBigInteger(2));
    tests.append(UnsignedBigInteger(3));
    tests.append(UnsignedBigInteger(5));
    tests.append(UnsignedBigInteger(7));
    tests.append(UnsignedBigInteger(11));
    tests.append(UnsignedBigInteger(13));
    UnsignedBigInteger seventeen { 17 };
    for (size_t i = tests.size(); i < 256; ++i) {
        tests.append(random_number(seventeen, p.minus(2)));
    }
    // Miller-Rabin's "error" is 8^-k. In adversarial cases, it's 4^-k.
    // With 200 random numbers, this would mean an error of about 2^-400.
    // So we don't need to worry too much about the quality of the random numbers.

    return MR_primality_test(p, tests);
}