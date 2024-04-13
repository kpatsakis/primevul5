UnsignedBigInteger ModularPower(const UnsignedBigInteger& b, const UnsignedBigInteger& e, const UnsignedBigInteger& m)
{
    if (m == 1)
        return 0;

    UnsignedBigInteger ep { e };
    UnsignedBigInteger base { b };
    UnsignedBigInteger exp { 1 };

    UnsignedBigInteger temp_1;
    UnsignedBigInteger temp_2;
    UnsignedBigInteger temp_3;
    UnsignedBigInteger temp_4;
    UnsignedBigInteger temp_multiply;
    UnsignedBigInteger temp_quotient;
    UnsignedBigInteger temp_remainder;

    while (!(ep < 1)) {
        if (ep.words()[0] % 2 == 1) {
            // exp = (exp * base) % m;
            UnsignedBigInteger::multiply_without_allocation(exp, base, temp_1, temp_2, temp_3, temp_4, temp_multiply);
            UnsignedBigInteger::divide_without_allocation(temp_multiply, m, temp_1, temp_2, temp_3, temp_4, temp_quotient, temp_remainder);
            exp.set_to(temp_remainder);
        }

        // ep = ep / 2;
        UnsignedBigInteger::divide_u16_without_allocation(ep, 2, temp_quotient, temp_remainder);
        ep.set_to(temp_quotient);

        // base = (base * base) % m;
        UnsignedBigInteger::multiply_without_allocation(base, base, temp_1, temp_2, temp_3, temp_4, temp_multiply);
        UnsignedBigInteger::divide_without_allocation(temp_multiply, m, temp_1, temp_2, temp_3, temp_4, temp_quotient, temp_remainder);
        base.set_to(temp_remainder);
    }
    return exp;
}