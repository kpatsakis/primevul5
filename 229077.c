TEST(ModMatchExpression, Equality1) {
    ModMatchExpression m1("a", 1, 2);
    ModMatchExpression m2("a", 2, 2);
    ModMatchExpression m3("a", 1, 1);
    ModMatchExpression m4("b", 1, 2);

    ASSERT(m1.equivalent(&m1));
    ASSERT(!m1.equivalent(&m2));
    ASSERT(!m1.equivalent(&m3));
    ASSERT(!m1.equivalent(&m4));
}