TEST(InMatchExpression, InMatchExpressionsWithUnequalCollatorsAreUnequal) {
    CollatorInterfaceMock collator1(CollatorInterfaceMock::MockType::kReverseString);
    InMatchExpression eq1("");
    eq1.setCollator(&collator1);
    CollatorInterfaceMock collator2(CollatorInterfaceMock::MockType::kAlwaysEqual);
    InMatchExpression eq2("");
    eq2.setCollator(&collator2);
    ASSERT(!eq1.equivalent(&eq2));
}