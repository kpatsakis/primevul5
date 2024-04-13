DEATH_TEST(LteOp, InvalidEooOperand, "Invariant failure _rhs") {
    BSONObj operand;
    LTEMatchExpression lte("", operand.firstElement());
}