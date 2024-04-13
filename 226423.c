TEST_F(QueryPlannerTest, CompoundIndexBoundsEqualityThenIn) {
    addIndex(BSON("a" << 1 << "b" << 1));
    runQuery(fromjson("{a: 5, b: {$in: [2,6,11]}}"));

    assertNumSolutions(2U);
    assertSolutionExists("{cscan: {dir: 1}}");
    assertSolutionExists(
        "{fetch: {filter: null, node: {ixscan: {filter: null, pattern: "
        "{a: 1, b: 1}, bounds: {a: [[5,5,true,true]], "
        "b:[[2,2,true,true],[6,6,true,true],[11,11,true,true]]}}}}}");
}