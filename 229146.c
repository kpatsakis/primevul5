TEST(RegexMatchExpression, Equality1) {
    RegexMatchExpression r1("a", "b", "");
    RegexMatchExpression r2("a", "b", "x");
    RegexMatchExpression r3("a", "c", "");
    RegexMatchExpression r4("b", "b", "");

    ASSERT(r1.equivalent(&r1));
    ASSERT(!r1.equivalent(&r2));
    ASSERT(!r1.equivalent(&r3));
    ASSERT(!r1.equivalent(&r4));
}