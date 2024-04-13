DEATH_TEST(LtOp, InvalidEooOperand, "Invariant failure _rhs") {
    BSONObj operand;
    LTMatchExpression lt("", operand.firstElement());
}