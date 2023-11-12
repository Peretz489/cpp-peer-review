enum class DateTimeStatus{
    VALID,
    INVALID_DATE,
    INVALID_TIME
}

DateTimeStatus CheckDateTimeValidity(const DateTime& dt) {

    constexpr bool is_leap_year = (dt.year % 4 == 0) && !(dt.year % 100 == 0 && dt.year % 400 != 0);
    static const array month_lengths = {31, 28 + is_leap_year, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    const uint max_year = 9999;

    if (dt.year < 1 || dt.year > max_year
    ||dt.month < 1||dt.month > 12
    ||dt.day < 1|| dt.day > month_lengths[dt.month - 1]) {
    return DateTimeStatus::INVALID_DATE;
    }
    if (dt.hour < 0|| dt.hour > 23
    ||dt.minute < 0||dt.minute > 59
    ||(dt.second < 0||dt.second > 59)) {
        return DateTimeStatus::INVALID_TIME;
    }
    return DateTimeStatus::VALID;
} 