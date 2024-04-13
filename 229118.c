TEST(InMatchExpression, InMatchExpressionsWithCollationEquivalentElementsAreEqual) {
    BSONObj obj1 = BSON(""
                        << "string1");
    BSONObj obj2 = BSON(""
                        << "string2");
    CollatorInterfaceMock collator1(CollatorInterfaceMock::MockType::kAlwaysEqual);
    InMatchExpression eq1("");
    eq1.setCollator(&collator1);
    CollatorInterfaceMock collator2(CollatorInterfaceMock::MockType::kAlwaysEqual);
    InMatchExpression eq2("");
    eq2.setCollator(&collator2);

    std::vector<BSONElement> equalities1{obj1.firstElement()};
    ASSERT_OK(eq1.setEqualities(std::move(equalities1)));

    std::vector<BSONElement> equalities2{obj2.firstElement()};
    ASSERT_OK(eq2.setEqualities(std::move(equalities2)));

    ASSERT(eq1.equivalent(&eq2));
}