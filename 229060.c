TEST(ComparisonMatchExpression, ComparisonMatchExpressionsWithEqualCollatorsAreEqual) {
    BSONObj operand = BSON("a" << 5);
    CollatorInterfaceMock collator1(CollatorInterfaceMock::MockType::kAlwaysEqual);
    EqualityMatchExpression eq1("a", operand["a"]);
    eq1.setCollator(&collator1);
    CollatorInterfaceMock collator2(CollatorInterfaceMock::MockType::kAlwaysEqual);
    EqualityMatchExpression eq2("a", operand["a"]);
    eq2.setCollator(&collator2);
    ASSERT(eq1.equivalent(&eq2));
}