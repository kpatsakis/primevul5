TEST(InMatchExpression, InMatchExpressionsWithDifferentNumbersOfElementsAreUnequal) {
    BSONObj obj = BSON(""
                       << "string");
    InMatchExpression eq1("");
    InMatchExpression eq2("");
    std::vector<BSONElement> equalities{obj.firstElement()};
    ASSERT_OK(eq1.setEqualities(std::move(equalities)));
    ASSERT(!eq1.equivalent(&eq2));
}