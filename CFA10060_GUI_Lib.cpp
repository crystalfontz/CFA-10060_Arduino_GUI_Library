/*********************************************************
Crystalfontz CFA-10060 Arduino Simple GUI Library

This library requires that the CFA10060_Lib is installed.

Crystalfontz: http://www.crystalfontz.com
Library Source: https://github.com/crystalfontz/CFA-10060_Arduino_GUI_Library

Distributed under the "Unlicense" software license.
See: http://unlicense.org/
**********************************************************/

#include <CFA10060_Lib.h>
#include "CFA10060_GUI_Lib.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

//See CFA10060_GUI_Lib.h for library settings.

///////////////////////////////////////////////////////////////////////////////////////////////////

CFA10060_GUI::CFA10060_GUI(CFA10060 &CFA10060_Lib)
{
	//class initialization
	display = &CFA10060_Lib;
	formItemCount = 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void CFA10060_GUI::newForm(void)
{
	//start a new form
	formItemCount = 0;
}

uint8_t CFA10060_GUI::addButton(itemType type,
	int8_t startX, uint8_t startY, uint8_t width, uint8_t height,
	CFA10060::Color_t bgColor, CFA10060::Color_t borderColor, CFA10060::Color_t textColor,
	const char *text)
{
	//add a button to the form
	if ((type != typeButtonYesNoOkCancel) && (type != typeButtonToggle))
		//not a button, return false
		return 0;
	if (formItemCount > CFA10060_GUI_MAX_FORM_ITEMS - 1)
		//already at max inputs, return false
		return 0;
	//save the button data
	formItem[formItemCount].type = type;
	formItem[formItemCount].x = startX;
	formItem[formItemCount].y = startY;
	formItem[formItemCount].w = width;
	formItem[formItemCount].h = height;
	formItem[formItemCount].bgColor = bgColor;
	formItem[formItemCount].borderColor = borderColor;
	formItem[formItemCount].textColor = textColor;
	formItem[formItemCount].text = text;
	formItem[formItemCount].state = stateNone;
	formItem[formItemCount].value = 0;
	if (formItem[formItemCount].type == typeButtonToggle)
		formItem[formItemCount].state = stateToggledOff;
	//pre-calcs
	formItem[formItemCount].asx = formItem[formItemCount].x;
	formItem[formItemCount].asy = formItem[formItemCount].y;
	formItem[formItemCount].aex = formItem[formItemCount].x + formItem[formItemCount].w;
	formItem[formItemCount].aey = formItem[formItemCount].y + formItem[formItemCount].h;
	//done ok
	formItemCount++;
	return 1;
}

uint8_t CFA10060_GUI::addSpinBox(
	int8_t startX, uint8_t startY, uint8_t width, uint8_t height,
	CFA10060::Color_t bgColor, CFA10060::Color_t borderColor, CFA10060::Color_t textColor,
	short startValue, short minValue, short maxValue)
{
	//add a number selection scroll box
	if (formItemCount > CFA10060_GUI_MAX_FORM_ITEMS - 1)
		//already at max inputs, return false
		return 0;
	//save item data
	formItem[formItemCount].type = typeSpinBox;
	formItem[formItemCount].x = startX;
	formItem[formItemCount].y = startY;
	formItem[formItemCount].w = width;
	formItem[formItemCount].h = height;
	formItem[formItemCount].bgColor = bgColor;
	formItem[formItemCount].borderColor = borderColor;
	formItem[formItemCount].textColor = textColor;
	formItem[formItemCount].min = minValue;
	formItem[formItemCount].max = maxValue;
	formItem[formItemCount].state = stateNone;
	formItem[formItemCount].value = startValue;
	//pre-calcs
	formItem[formItemCount].asx = formItem[formItemCount].x + 2;
	formItem[formItemCount].asy = formItem[formItemCount].y + 2;
	formItem[formItemCount].aex = formItem[formItemCount].x + formItem[formItemCount].h;
	formItem[formItemCount].aey = formItem[formItemCount].y + formItem[formItemCount].h - 2;
	formItem[formItemCount].bsx = formItem[formItemCount].x + formItem[formItemCount].w - formItem[formItemCount].h;
	formItem[formItemCount].bsy = formItem[formItemCount].y + 2;
	formItem[formItemCount].bex = formItem[formItemCount].x + formItem[formItemCount].w - 2;
	formItem[formItemCount].bey = formItem[formItemCount].y + formItem[formItemCount].h - 2;
	//done ok
	formItemCount++;
	return 1;
}

CFA10060_GUI::itemState CFA10060_GUI::getState(uint8_t item)
{
	//return item state
	return formItem[item].state;
 }

int CFA10060_GUI::getValue(uint8_t item)
{
	//return item value
	return formItem[item].value;
}

void CFA10060_GUI::runForm(void)
{
	//run the form
	uint8_t				cx, cy;
	CFA10060::Pads_t	pads;
	uint8_t				i;
	uint8_t				exit;

	//draw the form
	renderForm();
	//display the cursor
	display->cmdLCDCursor(CFA10060::cursorArrowNormal);

	exit = 0;
	while(!exit)
	{
		//get cursor location
		display->cmdLCDCursor(cx, cy, pads);
		//check button/pad state
		if (pads == CFA10060::Enter)
		{
			//enter tapped, check form item bounds
			for (i = 0; i < formItemCount; i++)
			{
				if ((formItem[i].type == typeButtonYesNoOkCancel) ||
					(formItem[i].type == typeButtonToggle))
				{
					//is a button or toggle button
					if ((formItem[i].asx < cx) &&
						(formItem[i].aex > cx) &&
						(formItem[i].asy < cy) &&
						(formItem[i].aey > cy))
					{
						//in bounds, this button was tapped
						if (formItem[i].type == typeButtonYesNoOkCancel)
						{
							//yes/no/ok/cancel style button, save state and exit form
							formItem[i].state = stateClicked;
							formItem[i].value = 1;
							exit = 1;
							break;
						}
						if (formItem[i].type == typeButtonToggle)
						{
							//toggle button, change state and redraw button
							if (formItem[i].state == stateToggledOn)
							{
								formItem[i].state = stateToggledOff;
								formItem[i].value = 0;
							}
							else
							{
								formItem[i].state = stateToggledOn;
								formItem[i].value = 1;
							}
							//redraw
							renderButton(i);
						}
					}
				}

				if (formItem[i].type == typeSpinBox)
				{
					//is a num scroll box
					if ((formItem[i].asx < cx) &&
						(formItem[i].aex > cx) &&
						(formItem[i].asy < cy) &&
						(formItem[i].aey > cy))
					{
						//in bounds, the -ve button was tapped
						formItem[i].value--;
						if (formItem[i].value < formItem[i].min)
							formItem[i].value = formItem[i].min;
						//redraw
						renderSpinBox(i);
						//change state
						formItem[i].state = stateClicked;
					}
					if ((formItem[i].bsx < cx) &&
						(formItem[i].bex > cx) &&
						(formItem[i].bsy < cy) &&
						(formItem[i].bey > cy))
					{
						//in bounds, the +ve button was tapped
						formItem[i].value++;
						if (formItem[i].value > formItem[i].max)
							formItem[i].value = formItem[i].max;
						//redraw
						renderSpinBox(i);
						//change state
						formItem[i].state = stateClicked;
					}
				}

			}
		}

		//small delay
		delay(100);
	}

	//finished, a button has been pressed
	//turn off the cursor
	display->cmdLCDCursor(CFA10060::cursorOff);
}

uint8_t CFA10060_GUI::renderForm(void)
{
	//render the described form
	uint8_t		i;

	//render form items
	for (i = 0; i < formItemCount; i++)
		switch (formItem[i].type)
		{
			case typeButtonYesNoOkCancel:
			case typeButtonToggle:
				renderButton(i);
				break;
			case typeSpinBox:
				renderSpinBox(i);
				break;
			default:
				break;
		}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

uint8_t CFA10060_GUI::renderButton(uint8_t b)
{
	//render the button
	uint8_t x, y;
	uint8_t cr;
	x = formItem[b].x;
	y = formItem[b].y;
	//button box
	display->cmdSetColor(formItem[b].borderColor, formItem[b].bgColor);
	display->cmdDrawRoundedRectangle(
		x, y,
		formItem[b].aex, formItem[b].aey,
		6, CFA10060::drawOutlineAndFilled);
	//selected/checked circle
	if (formItem[b].type == typeButtonToggle)
	{
		//circle radius
		if ((formItem[b].w) < (formItem[b].h))
			cr = formItem[b].w - 4;
		else
			cr = formItem[b].h - 4;
		cr = cr / 2;
		//draw circle
		if (formItem[b].state == stateToggledOn)
			display->cmdSetColor(formItem[b].borderColor, formItem[b].textColor);
		else
			display->cmdSetColor(formItem[b].borderColor, formItem[b].bgColor);
		display->cmdDrawCircle(x + cr + 2, y + cr + 2, cr, CFA10060::drawOutlineAndFilled);
		//move text over
		x += cr + 4;
	}
	//text center
	x += (formItem[b].w - (strlen(formItem[b].text) * 8)) / 2;
	y += (formItem[b].h - 8) / 2;
	//draw text
	display->cmdSetColor(
		formItem[b].textColor.R, formItem[b].textColor.G, formItem[b].textColor.B,
		0, 0, 0);
	display->cmdDrawText(x,y, (char*)formItem[b].text, CFA10060::font8x8, CFA10060::bgTransparent);
}

uint8_t CFA10060_GUI::renderSpinBox(uint8_t b)
{
	//render the number scroll box
	char		text[6];
	//outside box
	display->cmdSetColor(formItem[b].borderColor, formItem[b].bgColor);
	display->cmdDrawRoundedRectangle(
		formItem[b].x, formItem[b].y,
		formItem[b].x + formItem[b].w, formItem[b].y + formItem[b].h,
		6, CFA10060::drawOutlineAndFilled);
	//button -
	display->cmdSetColor(formItem[b].textColor, formItem[b].bgColor);
	display->cmdDrawRoundedRectangle(
		formItem[b].asx, formItem[b].asy,
		formItem[b].aex, formItem[b].aey,
		3, CFA10060::drawOutlineAndFilled);
	display->cmdDrawText(
		formItem[b].asx + (formItem[b].h / 2) - 3, formItem[b].asy + (formItem[b].h / 2) - 6,
		"-", CFA10060::font8x8, CFA10060::bgTransparent);
	//button +
	display->cmdDrawRoundedRectangle(
		formItem[b].bsx, formItem[b].bsy,
		formItem[b].bex, formItem[b].bey,
		3, CFA10060::drawOutlineAndFilled);
	display->cmdDrawText(
		formItem[b].bsx + (formItem[b].h / 2) - 3, formItem[b].bsy + (formItem[b].h / 2) - 6,
		"+", CFA10060::font8x8, CFA10060::bgTransparent);
	//number value
	itoa(formItem[b].value, text, 10);
	display->cmdSetColor(formItem[b].textColor, formItem[b].bgColor);
	display->cmdDrawText(
		formItem[b].x + ((formItem[b].w - strlen(text) * 8) / 2) + 2, formItem[b].y + (formItem[b].h / 2) - 4,
		text, CFA10060::font8x8, CFA10060::bgTransparent);
}
