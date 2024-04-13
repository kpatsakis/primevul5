    ConversionTable() {
        //
        // Conversions from NumberDouble
        //
        table[BSONType::NumberDouble][BSONType::NumberDouble] = &performIdentityConversion;
        table[BSONType::NumberDouble][BSONType::String] = &performFormatDouble;
        table[BSONType::NumberDouble][BSONType::Bool] = [](ExpressionContext* const expCtx,
                                                           Value inputValue) {
            return Value(inputValue.coerceToBool());
        };
        table[BSONType::NumberDouble][BSONType::Date] = &performCastNumberToDate;
        table[BSONType::NumberDouble][BSONType::NumberInt] = &performCastDoubleToInt;
        table[BSONType::NumberDouble][BSONType::NumberLong] = &performCastDoubleToLong;
        table[BSONType::NumberDouble][BSONType::NumberDecimal] = [](ExpressionContext* const expCtx,
                                                                    Value inputValue) {
            return Value(inputValue.coerceToDecimal());
        };

        //
        // Conversions from String
        //
        table[BSONType::String][BSONType::NumberDouble] = &parseStringToNumber<double, 0>;
        table[BSONType::String][BSONType::String] = &performIdentityConversion;
        table[BSONType::String][BSONType::jstOID] = &parseStringToOID;
        table[BSONType::String][BSONType::Bool] = &performConvertToTrue;
        table[BSONType::String][BSONType::Date] = [](ExpressionContext* const expCtx,
                                                     Value inputValue) {
            return Value(expCtx->timeZoneDatabase->fromString(inputValue.getStringData(),
                                                              mongo::TimeZoneDatabase::utcZone()));
        };
        table[BSONType::String][BSONType::NumberInt] = &parseStringToNumber<int, 10>;
        table[BSONType::String][BSONType::NumberLong] = &parseStringToNumber<long long, 10>;
        table[BSONType::String][BSONType::NumberDecimal] = &parseStringToNumber<Decimal128, 0>;

        //
        // Conversions from jstOID
        //
        table[BSONType::jstOID][BSONType::String] = [](ExpressionContext* const expCtx,
                                                       Value inputValue) {
            return Value(inputValue.getOid().toString());
        };
        table[BSONType::jstOID][BSONType::jstOID] = &performIdentityConversion;
        table[BSONType::jstOID][BSONType::Bool] = &performConvertToTrue;
        table[BSONType::jstOID][BSONType::Date] = [](ExpressionContext* const expCtx,
                                                     Value inputValue) {
            return Value(inputValue.getOid().asDateT());
        };

        //
        // Conversions from Bool
        //
        table[BSONType::Bool][BSONType::NumberDouble] = [](ExpressionContext* const expCtx,
                                                           Value inputValue) {
            return inputValue.getBool() ? Value(1.0) : Value(0.0);
        };
        table[BSONType::Bool][BSONType::String] = [](ExpressionContext* const expCtx,
                                                     Value inputValue) {
            return inputValue.getBool() ? Value("true"_sd) : Value("false"_sd);
        };
        table[BSONType::Bool][BSONType::Bool] = &performIdentityConversion;
        table[BSONType::Bool][BSONType::NumberInt] = [](ExpressionContext* const expCtx,
                                                        Value inputValue) {
            return inputValue.getBool() ? Value(int{1}) : Value(int{0});
        };
        table[BSONType::Bool][BSONType::NumberLong] = [](ExpressionContext* const expCtx,
                                                         Value inputValue) {
            return inputValue.getBool() ? Value(1LL) : Value(0LL);
        };
        table[BSONType::Bool][BSONType::NumberDecimal] = [](ExpressionContext* const expCtx,
                                                            Value inputValue) {
            return inputValue.getBool() ? Value(Decimal128(1)) : Value(Decimal128(0));
        };

        //
        // Conversions from Date
        //
        table[BSONType::Date][BSONType::NumberDouble] = [](ExpressionContext* const expCtx,
                                                           Value inputValue) {
            return Value(static_cast<double>(inputValue.getDate().toMillisSinceEpoch()));
        };
        table[BSONType::Date][BSONType::String] = [](ExpressionContext* const expCtx,
                                                     Value inputValue) {
            auto dateString = uassertStatusOK(TimeZoneDatabase::utcZone().formatDate(
                Value::kISOFormatString, inputValue.getDate()));
            return Value(dateString);
        };
        table[BSONType::Date][BSONType::Bool] = [](ExpressionContext* const expCtx,
                                                   Value inputValue) {
            return Value(inputValue.coerceToBool());
        };
        table[BSONType::Date][BSONType::Date] = &performIdentityConversion;
        table[BSONType::Date][BSONType::NumberLong] = [](ExpressionContext* const expCtx,
                                                         Value inputValue) {
            return Value(inputValue.getDate().toMillisSinceEpoch());
        };
        table[BSONType::Date][BSONType::NumberDecimal] = [](ExpressionContext* const expCtx,
                                                            Value inputValue) {
            return Value(
                Decimal128(static_cast<int64_t>(inputValue.getDate().toMillisSinceEpoch())));
        };

        //
        // Conversions from NumberInt
        //
        table[BSONType::NumberInt][BSONType::NumberDouble] = [](ExpressionContext* const expCtx,
                                                                Value inputValue) {
            return Value(inputValue.coerceToDouble());
        };
        table[BSONType::NumberInt][BSONType::String] = [](ExpressionContext* const expCtx,
                                                          Value inputValue) {
            return Value(static_cast<std::string>(str::stream() << inputValue.getInt()));
        };
        table[BSONType::NumberInt][BSONType::Bool] = [](ExpressionContext* const expCtx,
                                                        Value inputValue) {
            return Value(inputValue.coerceToBool());
        };
        table[BSONType::NumberInt][BSONType::NumberInt] = &performIdentityConversion;
        table[BSONType::NumberInt][BSONType::NumberLong] = [](ExpressionContext* const expCtx,
                                                              Value inputValue) {
            return Value(static_cast<long long>(inputValue.getInt()));
        };
        table[BSONType::NumberInt][BSONType::NumberDecimal] = [](ExpressionContext* const expCtx,
                                                                 Value inputValue) {
            return Value(inputValue.coerceToDecimal());
        };

        //
        // Conversions from NumberLong
        //
        table[BSONType::NumberLong][BSONType::NumberDouble] = [](ExpressionContext* const expCtx,
                                                                 Value inputValue) {
            return Value(inputValue.coerceToDouble());
        };
        table[BSONType::NumberLong][BSONType::String] = [](ExpressionContext* const expCtx,
                                                           Value inputValue) {
            return Value(static_cast<std::string>(str::stream() << inputValue.getLong()));
        };
        table[BSONType::NumberLong][BSONType::Bool] = [](ExpressionContext* const expCtx,
                                                         Value inputValue) {
            return Value(inputValue.coerceToBool());
        };
        table[BSONType::NumberLong][BSONType::Date] = &performCastNumberToDate;
        table[BSONType::NumberLong][BSONType::NumberInt] = &performCastLongToInt;
        table[BSONType::NumberLong][BSONType::NumberLong] = &performIdentityConversion;
        table[BSONType::NumberLong][BSONType::NumberDecimal] = [](ExpressionContext* const expCtx,
                                                                  Value inputValue) {
            return Value(inputValue.coerceToDecimal());
        };

        //
        // Conversions from NumberDecimal
        //
        table[BSONType::NumberDecimal][BSONType::NumberDouble] = &performCastDecimalToDouble;
        table[BSONType::NumberDecimal][BSONType::String] = [](ExpressionContext* const expCtx,
                                                              Value inputValue) {
            return Value(inputValue.getDecimal().toString());
        };
        table[BSONType::NumberDecimal][BSONType::Bool] = [](ExpressionContext* const expCtx,
                                                            Value inputValue) {
            return Value(inputValue.coerceToBool());
        };
        table[BSONType::NumberDecimal][BSONType::Date] = &performCastNumberToDate;
        table[BSONType::NumberDecimal][BSONType::NumberInt] = [](ExpressionContext* const expCtx,
                                                                 Value inputValue) {
            return performCastDecimalToInt(BSONType::NumberInt, inputValue);
        };
        table[BSONType::NumberDecimal][BSONType::NumberLong] = [](ExpressionContext* const expCtx,
                                                                  Value inputValue) {
            return performCastDecimalToInt(BSONType::NumberLong, inputValue);
        };
        table[BSONType::NumberDecimal][BSONType::NumberDecimal] = &performIdentityConversion;

        //
        // Miscellaneous conversions to Bool
        //
        table[BSONType::Object][BSONType::Bool] = &performConvertToTrue;
        table[BSONType::Array][BSONType::Bool] = &performConvertToTrue;
        table[BSONType::BinData][BSONType::Bool] = &performConvertToTrue;
        table[BSONType::RegEx][BSONType::Bool] = &performConvertToTrue;
        table[BSONType::DBRef][BSONType::Bool] = &performConvertToTrue;
        table[BSONType::Code][BSONType::Bool] = &performConvertToTrue;
        table[BSONType::Symbol][BSONType::Bool] = &performConvertToTrue;
        table[BSONType::CodeWScope][BSONType::Bool] = &performConvertToTrue;
        table[BSONType::bsonTimestamp][BSONType::Bool] = &performConvertToTrue;
    }