TEST_F(QueryPlannerTest, NegatedRangeIntGT) {
    addIndex(BSON("i" << 1));
    runQuery(fromjson("{i: {$not: {$gt: 5}}}"));

    assertNumSolutions(2U);
    assertSolutionExists("{cscan: {dir: 1}}");
    assertSolutionExists(
        "{fetch: {filter: null, node: {ixscan: {pattern: {i:1}, "
        "bounds: {i: [['MinKey',5,true,true], "
        "[Infinity,'MaxKey',false,true]]}}}}}");
}