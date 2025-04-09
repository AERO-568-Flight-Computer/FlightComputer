#include "Arduino.h"
#include "AMS.h"
#include "Wire.h"

AMS::AMS(int type, int address, int pmin, int pmax) {
	_type = type;
	_address = address;
	_pmin = pmin;
	_pmax = pmax;
}

bool AMS::Available(){
	switch (_type) {
		case 5812:
			_data = Wire.requestFrom(_address, 4); //request for bytes of data from the given address
			if (Wire.available() == 4) {
				_available = true;
			} else {
				_available = false;
			}
			break;
		case 5915:
			_data = Wire.requestFrom(_address, 4); //request for bytes of data from the given address
			if (Wire.available() == 4) {
				_available = true;
			} else {
				_available = false;
			}
			break;
		case 6915:
			_data = Wire.requestFrom(_address, 4); //request for bytes of data from the given address
			if (Wire.available() == 4) {
				_available = true;
			} else {
				_available = false;
			}
			break;
		default: 
			_available = false;
			break;
	}
	return _available;
}

float AMS::readPressure() {
	switch (_type) {
		case 5812:
			ReadFromWire(_address, _pressureMSB, _pressureLSB, _tempMSB, _tempLSB);
			_pressure = ((256*_pressureMSB+_pressureLSB)-3277.0)*(_pmax-_pmin)/26214+_pmin; //convert the pressure data into pressure in mbar, see AMS 5812 datasheet
			break;
		case 5915:
			ReadFromWire(_address, _pressureMSB, _pressureLSB, _tempMSB, _tempLSB);
			_pressure = ((256*(_pressureMSB&0x3F)+_pressureLSB)-1638.0)*(_pmax-_pmin)/13107+_pmin; //convert the pressure data into pressure in mbar, see AMS 5812 datasheet
			break;
		case 6915:
			ReadFromWire(_address, _pressureMSB, _pressureLSB, _tempMSB, _tempLSB);
			_pressure = ((256*(_pressureMSB&0x3F)+_pressureLSB)-1638.0)*(_pmax-_pmin)/13107+_pmin; //convert the pressure data into pressure in mbar, see AMS 5812 datasheet
			break;
		default:
			_pressure = NAN;
			break;
	}
	return _pressure;
}

float AMS::readTemperature() {
	switch (_type) {
		case 5812:
			ReadFromWire(_address, _pressureMSB, _pressureLSB, _tempMSB, _tempLSB);	
			_temperature = (((256*_tempMSB+_tempLSB)-3277.0)/238.309)-25; //convert the temperature data into temperature in degree celsius
			break;
		case 5915:
			ReadFromWire(_address, _pressureMSB, _pressureLSB, _tempMSB, _tempLSB);	
			_temperature = (((256.0*_tempMSB+_tempLSB)*200.0)/65536)-50; //convert the temperature data into temperature in degree celsius
			break;
		case 6915:
			ReadFromWire(_address, _pressureMSB, _pressureLSB, _tempMSB, _tempLSB);	
			_temperature = (((256.0*_tempMSB+_tempLSB)*200.0)/65536)-50; //convert the temperature data into temperature in degree celsius
			break;
		default:
			_temperature = NAN;
			break;
	}
	return _temperature;
}

float AMS::readSensor(float &pressure, float &temperature) {
	switch (_type) {
		case 5812:
			ReadFromWire(_address, _pressureMSB, _pressureLSB, _tempMSB, _tempLSB);	
			pressure = ((256*_pressureMSB+_pressureLSB)-3277.0)*(_pmax-_pmin)/26214+_pmin;
			temperature = (((256*_tempMSB+_tempLSB)-3277.0)/238.309)-25; //convert the temperature data into temperature in degree celsius
			break;
		case 5915:
			ReadFromWire(_address, _pressureMSB, _pressureLSB, _tempMSB, _tempLSB);
			pressure = (((256*(_pressureMSB&0x3F)+_pressureLSB)-1638.0)*(_pmax-_pmin)/13107)+_pmin; //convert the pressure data into pressure in mbar, see AMS 5812 datasheet
			temperature = (((256.0*_tempMSB+_tempLSB)*200.0)/65536)-50; //convert the temperature data into temperature in degree celsius
			break;
		case 6915:
			ReadFromWire(_address, _pressureMSB, _pressureLSB, _tempMSB, _tempLSB);
			pressure = (((256*(_pressureMSB&0x3F)+_pressureLSB)-1638.0)*(_pmax-_pmin)/13107)+_pmin; //convert the pressure data into pressure in mbar, see AMS 5812 datasheet
			temperature = (((256.0*_tempMSB+_tempLSB)*200.0)/65536)-50; //convert the temperature data into temperature in degree celsius
			break;
		default:
			pressure = NAN;
			temperature = NAN;
			break;
	}
}

void AMS::ReadFromWire(int address, int &pressureM, int &pressureL, int &tempM, int &tempL) {
	int data = Wire.requestFrom(address, 4); //request for bytes of data from the given address
	pressureM = Wire.read(); //read the first byte
	pressureL = Wire.read(); //read the second byte
	tempM = Wire.read(); //read the third byte
	tempL = Wire.read(); //read the fourth byte
}