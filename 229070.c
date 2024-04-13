TEST(ComparisonMatchExpression, StringMatchingRespectsCollation) {
    BSONObj operand = BSON("a"
                           << "string");
    CollatorInterfaceMock collator(CollatorInterfaceMock::MockType::kAlwaysEqual);
    EqualityMatchExpression eq("a", operand["a"]);
    eq.setCollator(&collator);
    ASSERT(eq.matchesBSON(BSON("a"
                               << "string2"),
                          NULL));
}