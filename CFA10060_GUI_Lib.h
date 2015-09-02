/*********************************************************
Crystalfontz CFA-10060 Arduino Simple GUI Library

This library requires that the CFA10060_Lib is installed.

Crystalfontz: http://www.crystalfontz.com
Library Source: https://github.com/crystalfontz/CFA-10060_Arduino_GUI_Library

Distributed under the "Unlicense" software license.
See: http://unlicense.org/
**********************************************************/

#ifndef CFA10060_GUI_LIB_H_
#define CFA10060_GUI_LIB_H_

#include "Arduino.h"
#include <CFA10060_Lib.h>

#include <avr/pgmspace.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <WString.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////

//library settings

//CFA10060_GUI_MAX_FORM_ITEMS sets the maximum number of controls on a form
// increasing ths number will increase memory usage by 33 bytes per item
#define CFA10060_GUI_MAX_FORM_ITEMS		(5)

////////////////////////////////////////////////////////////////////////////////////////////////////////////

//library class
class CFA10060_GUI
{
public:
	//public structures
	typedef enum
	{
		typeButtonYesNoOkCancel,
		typeButtonToggle,
		typeSpinBox
	} itemType;

	typedef enum
	{
		stateNone,
		stateClicked,
		stateToggledOff,
		stateToggledOn,
	} itemState;

	//public functions

	/*
		Initlizer for this CFA10060_GUI class.
		Make sure the CFA10060::init() function is run before using
		any of these GUI function.
	*/
	CFA10060_GUI(CFA10060 &CFA10060_Lib);

	/*
		Create a new blank form.
	*/
	void newForm();

	/*
		Execute the form.
		This function only returns when a button with typeButtonYesNoOkCancel is pressed.
	*/
	void runForm(void);

	/*
		Add a button of specfied type to the form.
		Returns 1 is successful.
	*/
	uint8_t addButton(itemType type, int8_t startX, uint8_t startY, uint8_t width, uint8_t height, CFA10060::Color_t bgColor, CFA10060::Color_t borderColor, CFA10060::Color_t textColor, const char *text);

	/*
		Add a spin box to the form.
		Returns 1 is successful.
	*/
	uint8_t addSpinBox(int8_t startX, uint8_t startY, uint8_t width, uint8_t height, CFA10060::Color_t bgColor, CFA10060::Color_t borderColor, CFA10060::Color_t textColor, short startValue, short minValue, short maxValue);

	//
	itemState getState(uint8_t item);
	int getValue(uint8_t item);

private:
	//private structures
	typedef struct
	{
		//settings
		itemType			type;
		uint8_t				x, y, w, h;
		CFA10060::Color_t	bgColor, borderColor, textColor;
		const char			*text;
		short				min;
		short				max;
		//state
		uint8_t				asx, asy, aex, aey;
		uint8_t				bsx, bsy, bex, bey;
		short				value;
		itemState			state;
	} FormItem_t; // = 33 bytes

	//private variables
	CFA10060		*display;
	uint8_t			formItemCount;
	FormItem_t		formItem[CFA10060_GUI_MAX_FORM_ITEMS];

	//private functions
	uint8_t renderForm(void);
	uint8_t renderButton(uint8_t b);
	uint8_t renderSpinBox(uint8_t b);
};

#endif
