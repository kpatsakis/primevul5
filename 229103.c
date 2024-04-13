TEST(EqOp, Equality1) {
    BSONObj operand = BSON("a" << 5 << "b" << 5 << "c" << 4);
    EqualityMatchExpression eq1("a", operand["a"]);
    EqualityMatchExpression eq2("a", operand["b"]);
    EqualityMatchExpression eq3("c", operand["c"]);

    ASSERT(eq1.equivalent(&eq1));
    ASSERT(eq1.equivalent(&eq2));
    ASSERT(!eq1.equivalent(&eq3));
}