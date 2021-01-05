#include <Arduino.h>
#include "MHZ19.h"
#include <SoftwareSerial.h> // Remove if using HardwareSerial

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define RX_PIN 10	  // Rx pin which the MHZ19 Tx pin is attached to
#define TX_PIN 11	  // Tx pin which the MHZ19 Rx pin is attached to
#define BAUDRATE 9600 // Device to MH-Z19 Serial baudrate (should not be changed)

MHZ19 myMHZ19;							 // Constructor for library
SoftwareSerial mySerial(RX_PIN, TX_PIN); // (Uno example) create device to MH-Z19 serial

LiquidCrystal_I2C lcd(0x27, 16, 2);

unsigned long getDataTimer = 0;

int latestCO2 = 0;

void setup()
{
	lcd.begin();
	lcd.backlight();
	lcd.clear();

	mySerial.begin(BAUDRATE); // (Uno example) device to MH-Z19 serial start
	myMHZ19.begin(mySerial);  // *Serial(Stream) refence must be passed to library begin().

	myMHZ19.autoCalibration(); // Turn auto calibration ON (OFF autoCalibration(false))
}

void loop()
{
	if (millis() - getDataTimer >= 2000)
	{
		int CO2;

		/* note: getCO2() default is command "CO2 Unlimited". This returns the correct CO2 reading even 
        if below background CO2 levels or above range (useful to validate sensor). You can use the 
        usual documented command with getCO2(false) */

		CO2 = myMHZ19.getCO2(); // Request CO2 (as ppm)

		if (CO2 != latestCO2)
		{
			int8_t temperature;
			temperature = myMHZ19.getTemperature();

			lcd.clear();
			lcd.print(CO2);
			lcd.print("ppm | ");

			if (temperature > 0)
			{
				lcd.print(temperature);
				lcd.print("(c)");
			}

			lcd.setCursor(0, 1);

			lcd.print(peopleReadable(CO2));
		}

		latestCO2 = CO2;
		getDataTimer = millis();
	}
}

const char *peopleReadable(int value)
{
	if (value > 4000 || value < 400)
	{
		return "Calibrating...";
	}

	if (value > 2500)
	{
		return "Alarm!";
	}

	if (value > 2000)
	{
		return "Fresh Time :(";
	}

	if (value > 1737)
	{
		return "Close to break";
	}

	if (value == 0)
	{
		return "Doesn't work";
	}

	return "It's fine ^_^";
}