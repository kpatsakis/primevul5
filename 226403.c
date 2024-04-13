TEST_F(QueryPlannerTest, NegatedRangeIntGTE) {
    addIndex(BSON("i" << 1));
    runQuery(fromjson("{i: {$not: {$gte: 5}}}"));

    assertNumSolutions(2U);
    assertSolutionExists("{cscan: {dir: 1}}");
    assertSolutionExists(
        "{fetch: {filter: null, node: {ixscan: {pattern: {i:1}, "
        "bounds: {i: [['MinKey',5,true,false], "
        "[Infinity,'MaxKey',false,true]]}}}}}");
}