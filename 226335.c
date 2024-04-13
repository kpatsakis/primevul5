TEST_F(QueryPlannerTest, CompoundIndexBoundsMiddleFieldMissing) {
    addIndex(BSON("a" << 1 << "b" << 1 << "c" << 1));
    runQuery(fromjson("{a: 1, c: {$lt: 3}}"));

    assertNumSolutions(2U);
    assertSolutionExists("{cscan: {dir: 1}}");
    assertSolutionExists(
        "{fetch: {node: {ixscan: {pattern: {a: 1, b: 1, c: 1}, bounds: "
        "{a: [[1,1,true,true]], b: [['MinKey','MaxKey',true,true]], "
        " c: [[-Infinity,3,true,false]]}}}}}");
}