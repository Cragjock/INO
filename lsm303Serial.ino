/*
The sensor outputs provided by the library are the raw 16-bit values
obtained by concatenating the 8-bit high and low accelerometer and
magnetometer data registers. They can be converted to units of g and
gauss using the conversion factors specified in the datasheet for your
particular device and full scale setting (gain).

Example: An LSM303D gives a magnetometer X axis reading of 1982 with
its default full scale setting of +/- 4 gauss. The M_GN specification
in the LSM303D datasheet (page 10) states a conversion factor of 0.160
mgauss/LSB (least significant bit) at this FS setting, so the raw
reading of -1982 corresponds to 1982 * 0.160 = 317.1 mgauss =
0.3171 gauss.

In the LSM303DLHC, LSM303DLM, and LSM303DLH, the acceleration data
registers actually contain a left-aligned 12-bit number, so the lowest
4 bits are always 0, and the values should be shifted right by 4 bits
(divided by 16) to be consistent with the conversion factors specified
in the datasheets.

Example: An LSM303DLH gives an accelerometer Z axis reading of -16144
with its default full scale setting of +/- 2 g. Dropping the lowest 4
bits gives a 12-bit raw value of -1009. The LA_So specification in the
LSM303DLH datasheet (page 11) states a conversion factor of 1 mg/digit
at this FS setting, so the value of -1009 corresponds to -1009 * 1 =
1009 mg = 1.009 g.
*/

#include <Wire.h>
#include <LSM303.h>

const double XL_gain PROGMEM = .000061;


LSM303 compass;

char report[80];
//char report1[80];


void setup()
{
  Serial.begin(9600);
  Wire.begin();
  compass.init();
  compass.enableDefault();
}

void loop()
{
  compass.read();
  //double z_axis = compass.a.z*XL_gain;
  //Serial.println(z_axis);

  String XL_x(compass.a.x*.000061);
  String XL_y(compass.a.y*.000061);
  String XL_z(compass.a.z*.000061);
  
 //snprintf(report1, sizeof(report1), "A: %6s %6s %6s    M: %6d %6d %6d",
 //   bufferax, bufferay, bufferaz,
 //   compass.m.x, compass.m.y, compass.m.z);
 // Serial.println(report1);

  //dtostrf(compass.a.x*.000061,6, 2, bufferax);
  //dtostrf(compass.a.y*.000061,6, 2, bufferay);
  //dtostrf(compass.a.z*.000061,6, 2, bufferaz);
  //dtostrf(compass.m.x,1, 2, bufferma);
  //dtostrf(compass.m.y,1, 2, buffermy);
  //dtostrf(compass.m.z,1, 2, buffermz);


  snprintf(report, sizeof(report), "A: %6s %6s %6s    M: %6d %6d %6d", 
      XL_x.c_str(), XL_y.c_str(), XL_z.c_str(),
      compass.m.x, compass.m.y, compass.m.z);
  Serial.print(F("adjusted: "));
  Serial.println(report);

  
/************************************
  snprintf(report, sizeof(report), "A: %6d %6d %6d    M: %6d %6d %6d",
    compass.a.x, compass.a.y, compass.a.z,
    compass.m.x, compass.m.y, compass.m.z);
  Serial.print(F("Raw data: ")); 
  Serial.println(report);
*********************************/ 
  delay(500);
}



