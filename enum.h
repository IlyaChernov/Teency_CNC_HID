#ifndef Enum_h
#define Enum_h

enum PositioningType
{
  G00,
  G01,
  G02,
  G03,
  PositioningTypeCount
};

enum CoordinateSystemType
{
  Absolute, //G90,
  Incremental, //G91,
  CoordinateSystemTypeCount
};

enum MetricSystemType
{
  Inch, //G20,
  Metric, //G21,
  MetricSystemTypeCount
};

#endif
