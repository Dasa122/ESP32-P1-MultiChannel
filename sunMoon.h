#pragma once

#include <Arduino.h>
#include <TimeLib.h>

class sunMoon {
public:
  void init(float timezoneMinutes, float latitude, float longitude) {
    timezoneOffsetMinutes = timezoneMinutes;
    lat = latitude;
    lon = longitude;
  }

  time_t sunRise() {
    return calculate(true);
  }

  time_t sunSet() {
    return calculate(false);
  }

private:
  float timezoneOffsetMinutes = 0.0f;
  float lat = 0.0f;
  float lon = 0.0f;

  static float normalizeDegrees(float value) {
    while (value < 0.0f) value += 360.0f;
    while (value >= 360.0f) value -= 360.0f;
    return value;
  }

  time_t calculate(bool isSunrise) {
    time_t current = now();
    if (current == 0) {
      current = makeTime({0, 0, 12, 1, 1, 2024 - 1970});
    }

    tmElements_t tm;
    breakTime(current, tm);

    int dayOfYear = dayOfYearFromDate(tm.Year + 1970, tm.Month, tm.Day);
    float lngHour = lon / 15.0f;
    float approximateTime = dayOfYear + ((isSunrise ? 6.0f : 18.0f) - lngHour) / 24.0f;
    float meanAnomaly = (0.9856f * approximateTime) - 3.289f;

    float sunLongitude = meanAnomaly + (1.916f * sin(radians(meanAnomaly))) + (0.020f * sin(radians(2.0f * meanAnomaly))) + 282.634f;
    sunLongitude = normalizeDegrees(sunLongitude);

    float rightAscension = degrees(atan(0.91764f * tan(radians(sunLongitude))));
    rightAscension = normalizeDegrees(rightAscension);

    float lQuadrant = floor(sunLongitude / 90.0f) * 90.0f;
    float raQuadrant = floor(rightAscension / 90.0f) * 90.0f;
    rightAscension += (lQuadrant - raQuadrant);
    rightAscension /= 15.0f;

    float sinDec = 0.39782f * sin(radians(sunLongitude));
    float cosDec = cos(asin(sinDec));
    float cosHour = (cos(radians(90.833f)) - (sinDec * sin(radians(lat)))) / (cosDec * cos(radians(lat)));

    if (cosHour > 1.0f || cosHour < -1.0f) {
      return current;
    }

    float hourAngle = isSunrise ? (360.0f - degrees(acos(cosHour))) : degrees(acos(cosHour));
    hourAngle /= 15.0f;

    float localMeanTime = hourAngle + rightAscension - (0.06571f * approximateTime) - 6.622f;
    float universalTime = localMeanTime - lngHour;
    float localTime = universalTime + (timezoneOffsetMinutes / 60.0f);

    while (localTime < 0.0f) localTime += 24.0f;
    while (localTime >= 24.0f) localTime -= 24.0f;

    int hourValue = (int)localTime;
    int minuteValue = (int)((localTime - hourValue) * 60.0f + 0.5f);
    if (minuteValue >= 60) {
      minuteValue -= 60;
      hourValue = (hourValue + 1) % 24;
    }

    tm.Hour = hourValue;
    tm.Minute = minuteValue;
    tm.Second = 0;
    return makeTime(tm);
  }

  static int dayOfYearFromDate(int yearValue, int monthValue, int dayValue) {
    static const int daysBeforeMonth[] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
    int dayOfYear = daysBeforeMonth[monthValue - 1] + dayValue;
    bool isLeapYear = ((yearValue % 4 == 0) && (yearValue % 100 != 0)) || (yearValue % 400 == 0);
    if (isLeapYear && monthValue > 2) {
      dayOfYear += 1;
    }
    return dayOfYear;
  }
};
