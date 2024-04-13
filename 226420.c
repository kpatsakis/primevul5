TEST_F(QueryPlannerTest, CompoundIndexBoundsRangeAndEquality) {
    addIndex(BSON("a" << 1 << "b" << 1));
    runQuery(fromjson("{a: {$gt: 8}, b: 6}"));

    assertNumSolutions(2U);
    assertSolutionExists("{cscan: {dir: 1}}");
    assertSolutionExists(
        "{fetch: {node: {ixscan: {pattern: {a: 1, b: 1}, bounds: "
        "{a: [[8,Infinity,false,true]], b:[[6,6,true,true]]}}}}}");
}