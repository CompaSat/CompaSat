#include <SoftwareSerial.h>
#include "ABlocks_TinyGPS.h"

TinyGPS gps;
float gps_lat=0;
float gps_long=0;
float gps_speed_kmph=0;
float gps_speed_mph=0;
float gps_altitude=0;
float gps_course=0;
bool gps_fixed=false;
byte gps_day=0;
byte gps_month=0;
int gps_year=0;
byte gps_hour=0;
byte gps_min=0;
byte gps_sec=0;
byte gps_hund=0;
SoftwareSerial gps_serial(3,4);
unsigned long task_time_ms=0;

void fnc_gps_update()
{
	unsigned long fix_age;
	while (gps_serial.available()){
		if(gps.encode(gps_serial.read())){
			gps_fixed=false;
			gps_lat=gps_long=gps_speed_kmph=gps_speed_mph=gps_course=0;
			gps_day=gps_month=gps_year=gps_hour=gps_min=gps_sec=0;
			gps.f_get_position(&gps_lat, &gps_long, &fix_age);
			if (fix_age != TinyGPS::GPS_INVALID_AGE && fix_age<10000){
				gps_fixed=true;
				gps.crack_datetime(&gps_year, &gps_month, &gps_day,&gps_hour, &gps_min, &gps_sec, &gps_hund, &fix_age);
				gps_altitude=gps.f_altitude();
				gps_course=gps.f_course();
				gps_speed_kmph=gps.f_speed_kmph();
				gps_speed_mph=gps.f_speed_mph();
			}
		}
	}
}

String fnc_gps_date_string(int _format){
	char buf[19];

	if(_format==1){	//yyyy/mm/dd
		 sprintf(buf, "%04d/%02d/%02d", gps_year, gps_month,gps_day);
	}
	if(_format==2){	//dd-mm-yyyy
		 sprintf(buf, "%02d-%02d-%04d", gps_day, gps_month,gps_year);
	}
	if(_format==3){	//dd/mm/yyy
		 sprintf(buf, "%02d/%02d/%04d", gps_day, gps_month,gps_year);
	}
	else{	//(default)	yyyy-mm-dd
		sprintf(buf, "%04d-%02d-%02d", gps_year, gps_month,gps_day);
	}

	return String(buf);
}

String fnc_gps_time_string(int _format){
	char buf[19];
	if(_format==1){	//hh:mm
		sprintf(buf, "%02d:%02d", gps_hour, gps_min);
	}
	else{	//(default)	hh:mm:ss
		sprintf(buf, "%02d:%02d:%02d", gps_hour, gps_min,gps_sec);
	}
	return String(buf);
}

void setup()
{


	Serial.begin(9600);
	Serial.flush();
	while(Serial.available()>0)Serial.read();

	gps_serial.begin(9600);

}


void loop()
{
	fnc_gps_update();
  	if((millis()-task_time_ms)>=5000){
  		task_time_ms=millis();
  		if (gps_fixed) {
  			Serial.print(String("FECHA"));
  			Serial.println(fnc_gps_date_string(0));
  			Serial.print(String("HORA"));
  			Serial.println(fnc_gps_time_string(0));
  			Serial.print(String("LAT: "));
  			Serial.println((String(gps_lat,6)));
  			Serial.print(String("LON: "));
  			Serial.println((String(gps_long,6)));
  		}
  		else {
  			Serial.println(String("NO HAY DATOS"));
  		}

  	}

}