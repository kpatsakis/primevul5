TEST_F(QueryPlannerTest, CantExplodeMetaSort) {
    addIndex(BSON("a" << 1 << "b" << 1 << "c"
                      << "text"));
    runQuerySortProj(fromjson("{a: {$in: [1, 2]}, b: {$in: [3, 4]}}"),
                     fromjson("{c: {$meta: 'textScore'}}"),
                     fromjson("{c: {$meta: 'textScore'}}"));

    assertNumSolutions(1U);
    assertSolutionExists(
        "{proj: {spec: {c:{$meta:'textScore'}}, node: "
        "{sort: {pattern: {c:{$meta:'textScore'}}, limit: 0, node: {sortKeyGen: {node: "
        "{cscan: {filter: {a:{$in:[1,2]},b:{$in:[3,4]}}, dir: 1}}}}}}}}");
}