#ifndef AMS_h
#define AMS_h

#include "Arduino.h"

class AMS {
	public:
		AMS(int type, int address, int pmin, int pmax);
		bool Available();
		float readPressure();
		float readTemperature();
		float readSensor(float &pressure, float &temperature);
	private:
		int _type;
		int _address;
		int _pmin;
		int _pmax;
		int _data;
		int _pressureMSB;
		int _pressureLSB;
		int _tempMSB;
		int _tempLSB;
		float _pressure;
		float _temperature;
		float _sensOutput[2];
		bool _available;
		void ReadFromWire(int address, int &pressureM, int &pressureL, int &tempM, int &tempL);
};
#endif