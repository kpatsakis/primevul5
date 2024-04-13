UnsignedBigInteger ModularInverse(const UnsignedBigInteger& a_, const UnsignedBigInteger& b)
{
    if (b == 1)
        return { 1 };

    UnsignedBigInteger one { 1 };
    UnsignedBigInteger temp_1;
    UnsignedBigInteger temp_2;
    UnsignedBigInteger temp_3;
    UnsignedBigInteger temp_4;
    UnsignedBigInteger temp_plus;
    UnsignedBigInteger temp_minus;
    UnsignedBigInteger temp_quotient;
    UnsignedBigInteger temp_remainder;
    UnsignedBigInteger d;

    auto a = a_;
    auto u = a;
    if (a.words()[0] % 2 == 0) {
        // u += b
        UnsignedBigInteger::add_without_allocation(u, b, temp_plus);
        u.set_to(temp_plus);
    }

    auto v = b;
    UnsignedBigInteger x { 0 };

    // d = b - 1
    UnsignedBigInteger::subtract_without_allocation(b, one, d);

    while (!(v == 1)) {
        while (v < u) {
            // u -= v
            UnsignedBigInteger::subtract_without_allocation(u, v, temp_minus);
            u.set_to(temp_minus);

            // d += x
            UnsignedBigInteger::add_without_allocation(d, x, temp_plus);
            d.set_to(temp_plus);

            while (u.words()[0] % 2 == 0) {
                if (d.words()[0] % 2 == 1) {
                    // d += b
                    UnsignedBigInteger::add_without_allocation(d, b, temp_plus);
                    d.set_to(temp_plus);
                }

                // u /= 2
                UnsignedBigInteger::divide_u16_without_allocation(u, 2, temp_quotient, temp_remainder);
                u.set_to(temp_quotient);

                // d /= 2
                UnsignedBigInteger::divide_u16_without_allocation(d, 2, temp_quotient, temp_remainder);
                d.set_to(temp_quotient);
            }
        }

        // v -= u
        UnsignedBigInteger::subtract_without_allocation(v, u, temp_minus);
        v.set_to(temp_minus);

        // x += d
        UnsignedBigInteger::add_without_allocation(x, d, temp_plus);
        x.set_to(temp_plus);

        while (v.words()[0] % 2 == 0) {
            if (x.words()[0] % 2 == 1) {
                // x += b
                UnsignedBigInteger::add_without_allocation(x, b, temp_plus);
                x.set_to(temp_plus);
            }

            // v /= 2
            UnsignedBigInteger::divide_u16_without_allocation(v, 2, temp_quotient, temp_remainder);
            v.set_to(temp_quotient);

            // x /= 2
            UnsignedBigInteger::divide_u16_without_allocation(x, 2, temp_quotient, temp_remainder);
            x.set_to(temp_quotient);
        }
    }

    // x % b
    UnsignedBigInteger::divide_without_allocation(x, b, temp_1, temp_2, temp_3, temp_4, temp_quotient, temp_remainder);
    return temp_remainder;
}