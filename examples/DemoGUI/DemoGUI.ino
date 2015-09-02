/*********************************************************
This is an example use of the Crystalfontz CFA10060
hardware and CFA10060 Arduino GUI library.

http://www.crystalfontz.com

Distributed under the "Unlicense" software license.
See: http://unlicense.org/
**********************************************************/

#include <CFA10060_Lib.h>
#include <CFA10060_GUI_Lib.h>

#include <Wire.h>
#include <SoftwareSerial.h>

//the CFA10060 device & gui libraries
CFA10060		display;
CFA10060_GUI	gui(display);

//setup function
void setup()
{
  delay(500);							//short sleep to stop debugging reset errors
  Serial.begin(115200);					//start serial at 115200 baud
  Serial.println("Doing setup...");
  Wire.begin();							//use the I2C bus
  display.init();						//initialize the CFA10060
  Serial.println("Setup done.");
}

//the main program
void loop()
{
	//local vars
	uint8_t						time, start_time;
	CFA10060_GUI::itemState		count_down;
	char						num_text[8];
	uint8_t						center;
	uint8_t						i;
	uint8_t						shade;

	//some GUI colors (r,g,b)
	const CFA10060::Color_t		black = {0, 0, 0};
	const CFA10060::Color_t		white = {255, 255, 255};
	const CFA10060::Color_t		grey = {128, 128, 128};
	const CFA10060::Color_t		dark_red = {100, 0, 0};
	const CFA10060::Color_t		green = {0, 255, 0};
	const CFA10060::Color_t		dark_green = {0, 100, 0};

	//lets ping the display first to make sure its present & alive
	Serial.println("Doing Ping...");
	if (!display.cmdPing("pingdata"))
		Serial.println("Ping FAILED.");
	else
	Serial.println("Ping OK.");

	//clear out any errors
	Serial.println("Clearing Errors...");
	display.cmdCheckAndClearFileSystemErrors();
	Serial.println("Cleared.");

	//turn off the cursor (just in case it was left on)
	display.cmdLCDCursor(CFA10060::cursorOff);
  
	Serial.println("Main Loop...");
	//main loop
	while (1)
	{
		//blank the LCD to black
		display.cmdFillSolidColor(black);

		//timer text
		display.cmdSetColor(green, black);
		display.cmdDrawText(64 - (5 * 6), 90, "TIMER", CFA10060::font12x16, CFA10060::bgTransparent);

		//create a new blank GUI form
		gui.newForm();

		//add controls to the form
		gui.addButton(CFA10060_GUI::typeButtonYesNoOkCancel, 5, 5, 54, 18, dark_green, white, white, "GO");		//item 0 
		gui.addButton(CFA10060_GUI::typeButtonYesNoOkCancel, 69, 5, 54, 18, dark_red, white, white, "RESET");	//item 1
		gui.addButton(CFA10060_GUI::typeButtonToggle, 10, 28, 108, 18, grey, white, black, "COUNT DOWN");		//item 2
		gui.addSpinBox(20, 51, 88, 26, grey, white, black, 5, 1, 60);											//item 3

		//execute the form (this returns when a typeButtonYesNoOkCancel button is pressed)
		gui.runForm();

		//get go/cancel button result
		if (gui.getState(0) == CFA10060_GUI::stateClicked)
		{
			//"GO" button was pressed
			
			//get the spin box value (time)
			start_time = (uint8_t)gui.getValue(3);
			//get count up/down setting
			count_down = gui.getState(2);

			//do count up/down timer
			time = start_time;
			display.cmdSetColor(green, black);
			while (time != 0)
			{
				//blank out the last number
				display.cmdDrawRectangle(0, 90, 127, 127, CFA10060::drawFilled);

				//convert time count into a string
				if (count_down == CFA10060_GUI::stateToggledOn)
					itoa(time, num_text, 10);
				else
					itoa(start_time - time, num_text, 10);

				//calc text color (moves from green to red)
				shade = (255 * (uint16_t)time) / start_time;
				display.cmdSetColor(255-shade,shade,0, 0,0,0);

				//calc position for text
				center = 64 - strlen(num_text) * 6; // 6 = half a font12x16 char width
				display.cmdDrawText(center, 90, num_text, CFA10060::font12x16, CFA10060::bgTransparent);

				//delay for 1 second
				_delay_ms(1000);

				//time count
				time--;
			}

			//count finished, flash finish screen
			for (i = 0; i < 6; i++)
			{
				display.cmdFillSolidColor(dark_green);
				delay(100);
				display.cmdFillSolidColor(dark_red);
				display.cmdSetColor(white, dark_green);
				display.cmdDrawRectangle(20,20, 107, 107, CFA10060::drawFilled);
				display.cmdDrawText(64-6*5, 64-8, "BANG!", CFA10060::font12x16, CFA10060::bgTransparent);
				delay(400);
			}

			//done
		}

		//clear out any errors
		display.cmdCheckAndClearFileSystemErrors();

		//loop back now (loop resets GUI)
	}

	//never gets to here
}
