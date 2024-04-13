TEST_F(QueryPlannerTest, EmptyQueryWithProjectionUsesCollscanIfIndexCollationDiffers) {
    params.options = QueryPlannerParams::GENERATE_COVERED_IXSCANS;
    CollatorInterfaceMock collator(CollatorInterfaceMock::MockType::kReverseString);
    addIndex(BSON("a" << 1), &collator);
    runQueryAsCommand(fromjson("{find: 'testns', projection: {_id: 0, a: 1}}"));
    assertNumSolutions(1);
    assertSolutionExists(
        "{proj: {spec: {_id: 0, a: 1}, node: "
        "{cscan: {dir: 1}}}}");
}