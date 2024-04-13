UnsignedBigInteger random_number(const UnsignedBigInteger& min, const UnsignedBigInteger& max_excluded)
{
    ASSERT(min < max_excluded);
    auto range = max_excluded.minus(min);
    UnsignedBigInteger base;
    auto size = range.trimmed_length() * sizeof(u32) + 2;
    // "+2" is intentional (see below).
    // Also, if we're about to crash anyway, at least produce a nice error:
    ASSERT(size < 8 * MiB);
    u8 buf[size];
    AK::fill_with_random(buf, size);
    UnsignedBigInteger random { buf, size };
    // At this point, `random` is a large number, in the range [0, 256^size).
    // To get down to the actual range, we could just compute random % range.
    // This introduces "modulo bias". However, since we added 2 to `size`,
    // we know that the generated range is at least 65536 times as large as the
    // required range! This means that the modulo bias is only 0.0015%, if all
    // inputs are chosen adversarially. Let's hope this is good enough.
    auto divmod = random.divided_by(range);
    // The proper way to fix this is to restart if `divmod.quotient` is maximal.
    return divmod.remainder.plus(min);
}