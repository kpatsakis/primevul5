TEST_F(QueryPlannerTest, NegationCantUseSparseIndex2) {
    // false means not multikey, true means sparse
    addIndex(BSON("i" << 1 << "j" << 1), false, true);
    runQuery(fromjson("{i: 4, j: {$ne: 5}}"));

    assertNumSolutions(2U);
    assertSolutionExists("{cscan: {dir: 1}}");
    assertSolutionExists(
        "{fetch: {node: {ixscan: {pattern: {i:1,j:1}, bounds: "
        "{i: [[4,4,true,true]], j: [['MinKey','MaxKey',true,true]]}}}}}");
}