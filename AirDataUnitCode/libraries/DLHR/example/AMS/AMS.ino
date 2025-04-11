#include <AMS.h> //include the AMS library

float Pressure; //declaration of the used variables, the variables pressure and temperature have to be float since readSensor, readPressure and readTemperature return a float value containing the current pressure value in the given pressure unit and the current temperature in degree celsius.
float Temperature; 
String DataString;
char PrintData[48];

AMS AMSa(6915, 0x56, -50, 50); //define the sensor's instance with the sensor's family, sensor's I2C address as well as its specified minimum and maximum pressure

void setup() { //initialization function which will only be executed once, when the Arduino is powered up
  Serial.begin(9600); //set the serial port's baud rate. The COM port's standard parameters are: Data bits: 8, Parity: none, Stop bits: 1, Flow control: none, Baud rate: 9600
}

void loop() { //main function, which will be repeated continuously until Arduino is resetted or removed from its power source and which contains the data readout from AMS 5812
  //Option 1: read pressure and temperature values
  if (AMSa.Available() == true) { //check if AMS 5812 responds to the given I2C address
    AMSa.readSensor(Pressure, Temperature); //read the sensor's temperature and pressure data. Please note that the temperature value contains the sensor's self heating as well as the ambient temperature
    if (isnan(Pressure) || isnan(Temperature)) { //check if an error occurred leading to the function returning a NaN
      Serial.write("Please check the sensor family name."); //write an error message on the serial port
    } else {
      DataString = String(Pressure) + " mbar " + String(Temperature) + " " + (char)176 + "C \n"; //put the data into a string
      DataString.toCharArray(PrintData, 48); //convert string into CharArray
      Serial.write(PrintData); //write the sensor's data on the serial port
    }
  } else {
    Serial.write("The sensor did not answer."); //tell the user that there is something wrong with the communication between Arduino and the sensor.
  }
  delay(1000); //wait for 1 s until reading new data from the Sensor

  //Option 2: read pressure values only
  if (AMSa.Available() == true) {
    Pressure = AMSa.readPressure(); //read AMS 5812's pressure data only
    if (isnan(Pressure)) {
      Serial.write("Please check the sensor family name.");
    } else {
      DataString = String(Pressure) + " mbar \n";
      DataString.toCharArray(PrintData, 48);
      Serial.write(PrintData);
    }
  } else {
    Serial.write("The sensor did not answer.");
  }
  delay(5);

  //Option 3: read temperature values only
  if (AMSa.Available() == true) {
    Temperature = AMSa.readTemperature(); //read AMS 5812's temperature data in degree celsius
    if (isnan(Temperature)) {
      Serial.write("Please check the sensor family name."); //
    } else {
      DataString = String(Temperature) + " " + (char)176 + "C \n";
      DataString.toCharArray(PrintData, 48);
      Serial.write(PrintData);
    }
  } else {
    Serial.write("The sensor did not answer.");
  }
  delay(1000);
}
