TEST_F(QueryPlannerTest, NoKeepWithGeoNear) {
    params.options = QueryPlannerParams::KEEP_MUTATIONS;
    addIndex(BSON("a"
                  << "2d"));
    runQuery(fromjson("{a: {$near: [0,0], $maxDistance:0.3 }}"));
    ASSERT_EQUALS(getNumSolutions(), 1U);
    assertSolutionExists("{geoNear2d: {a: '2d'}}");
}