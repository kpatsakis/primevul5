TEST_F(QueryPlannerTest, NegatedRangeStrGTE) {
    addIndex(BSON("i" << 1));
    runQuery(fromjson("{i: {$not: {$gte: 'a'}}}"));

    assertNumSolutions(2U);
    assertSolutionExists("{cscan: {dir: 1}}");
    assertSolutionExists(
        "{fetch: {filter: null, node: {ixscan: {pattern: {i:1}, "
        "bounds: {i: [['MinKey','a',true,false], "
        "[{},'MaxKey',true,true]]}}}}}");
}