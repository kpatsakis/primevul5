UnsignedBigInteger GCD(const UnsignedBigInteger& a, const UnsignedBigInteger& b)
{
    UnsignedBigInteger temp_a;
    UnsignedBigInteger temp_b;
    UnsignedBigInteger temp_1;
    UnsignedBigInteger temp_2;
    UnsignedBigInteger temp_3;
    UnsignedBigInteger temp_4;
    UnsignedBigInteger temp_quotient;
    UnsignedBigInteger temp_remainder;
    UnsignedBigInteger output;

    GCD_without_allocation(a, b, temp_a, temp_b, temp_1, temp_2, temp_3, temp_4, temp_quotient, temp_remainder, output);

    return output;
}