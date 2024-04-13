TEST(ExistsMatchExpression, Equivalent) {
    ExistsMatchExpression e1("a");
    ExistsMatchExpression e2("b");

    ASSERT(e1.equivalent(&e1));
    ASSERT(!e1.equivalent(&e2));
}