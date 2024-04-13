TEST_F(QueryPlannerTest, NToReturnHackWithFindCommand) {
    params.options |= QueryPlannerParams::SPLIT_LIMITED_SORT;

    runQueryAsCommand(fromjson("{find: 'testns', sort: {a:1}, ntoreturn:3}"));

    assertNumSolutions(1U);
    assertSolutionExists(
        "{ensureSorted: {pattern: {a: 1}, node: "
        "{or: {nodes: ["
        "{sort: {limit:3, pattern: {a:1}, node: {sortKeyGen: {node: {cscan: {dir:1}}}}}}, "
        "{sort: {limit:0, pattern: {a:1}, node: {sortKeyGen: {node: {cscan: {dir:1}}}}}}"
        "]}}}}");
}