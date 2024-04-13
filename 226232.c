TEST_F(QueryPlannerTest, NToReturnHackWithSingleBatch) {
    params.options |= QueryPlannerParams::SPLIT_LIMITED_SORT;

    runQueryAsCommand(fromjson("{find: 'testns', sort: {a:1}, ntoreturn:3, singleBatch:true}"));

    assertNumSolutions(1U);
    assertSolutionExists(
        "{sort: {pattern: {a:1}, limit:3, node: {sortKeyGen: {node: "
        "{cscan: {dir:1, filter: {}}}}}}}");
}