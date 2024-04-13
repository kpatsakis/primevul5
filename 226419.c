TEST_F(QueryPlannerTest, CompoundAndNonCompoundIndices) {
    addIndex(BSON("a" << 1));
    addIndex(BSON("a" << 1 << "b" << 1), true);
    runQuery(fromjson("{a: 1, b: {$gt: 2, $lt: 2}}"));

    ASSERT_EQUALS(getNumSolutions(), 3U);
    assertSolutionExists("{cscan: {dir: 1}}");
    assertSolutionExists(
        "{fetch: {filter: {$and:[{b:{$lt:2}},{b:{$gt:2}}]}, node: "
        "{ixscan: {pattern: {a:1}, bounds: {a: [[1,1,true,true]]}}}}}");
    assertSolutionExists(
        "{fetch: {filter: {b:{$gt:2}}, node: "
        "{ixscan: {pattern: {a:1,b:1}, bounds: "
        "{a: [[1,1,true,true]], b: [[-Infinity,2,true,false]]}}}}}");
}