TEST_F(QueryPlannerTest, CompoundIndexBoundsLastFieldMissing) {
    addIndex(BSON("a" << 1 << "b" << 1 << "c" << 1));
    runQuery(fromjson("{a: 5, b: {$gt: 7}}"));

    assertNumSolutions(2U);
    assertSolutionExists("{cscan: {dir: 1}}");
    assertSolutionExists(
        "{fetch: {node: {ixscan: {pattern: {a: 1, b: 1, c: 1}, bounds: "
        "{a: [[5,5,true,true]], b: [[7,Infinity,false,true]], "
        " c: [['MinKey','MaxKey',true,true]]}}}}}");
}