static void GCD_without_allocation(
    const UnsignedBigInteger& a,
    const UnsignedBigInteger& b,
    UnsignedBigInteger& temp_a,
    UnsignedBigInteger& temp_b,
    UnsignedBigInteger& temp_1,
    UnsignedBigInteger& temp_2,
    UnsignedBigInteger& temp_3,
    UnsignedBigInteger& temp_4,
    UnsignedBigInteger& temp_quotient,
    UnsignedBigInteger& temp_remainder,
    UnsignedBigInteger& output)
{
    temp_a.set_to(a);
    temp_b.set_to(b);
    for (;;) {
        if (temp_a == 0) {
            output.set_to(temp_b);
            return;
        }

        // temp_b %= temp_a
        UnsignedBigInteger::divide_without_allocation(temp_b, temp_a, temp_1, temp_2, temp_3, temp_4, temp_quotient, temp_remainder);
        temp_b.set_to(temp_remainder);
        if (temp_b == 0) {
            output.set_to(temp_a);
            return;
        }

        // temp_a %= temp_b
        UnsignedBigInteger::divide_without_allocation(temp_a, temp_b, temp_1, temp_2, temp_3, temp_4, temp_quotient, temp_remainder);
        temp_a.set_to(temp_remainder);
    }
}