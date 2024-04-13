TEST(ComparisonMatchExpression, StringMatchingWithNullCollatorUsesBinaryComparison) {
    BSONObj operand = BSON("a"
                           << "string");
    EqualityMatchExpression eq("a", operand["a"]);
    ASSERT(!eq.matchesBSON(BSON("a"
                                << "string2"),
                           NULL));
}