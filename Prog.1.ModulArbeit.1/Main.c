#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define KEY_ESCAPE  0x001b
#define KEY_ENTER   0x000d // 13
#define KEY_DELETE  0x0008 // 8
#define TWO_BYTE_IN 0x00e0 //224
#define KEY_UP      0x0048 // arr 72
#define KEY_DOWN    0x0050 // arr 80
#define KEY_LEFT    0x004b // arr 75
#define KEY_RIGHT   0x004d // arr 77

#define COLOR_SELECTED_RES "\033[0;30m"
#define COLOR_SELECTED "\033[5;34m"
#define COLOR_ACTIVE "\033[1;107m"
#define BG_COLOR "\x1b[47m\033[1;30m"

//Limit of our input since Number with more then 20 digits are already bigger then what can be in a long long so we dont have store those. 
#define INPUT_BUFFER_SIZE 32

enum STATE {
	CONV_DEZ_HEX = 0,
	CONV_HEX_DEZ,
	INPUT_NUMBER,
	END,
};

typedef struct  {
	//Well this part is a bit wacky since, I'm misusing a boal as a State, but since we only have 2 modi it works and makes later check simpler to write.
  // true = Decimal -> Hex, false = Hex -> Decimal
	bool ConvMode;
	char* ErrorMsg;
	char* ConvResult;
	char InputBuffer[INPUT_BUFFER_SIZE];
	int BufferPosIdx;
	enum STATE CurrSelection;
}MenuData;
#pragma region Converter


void ConvDezHex(char* dez, char* o_Res) {
	o_Res = *dez;
}
void ConvHexDez(char* hex, char* o_Res) {
	o_Res = *hex;
}

#pragma endregion


#pragma region  Menu



void DrawAcceptMenu(bool state, char* warningMsg) {
	puts("\x1b(0lqqqqqqqqqqqqq\x1b(B Do you want to Continue ? \x1b(0qqqqqqqqqqqqqk\x1b(B");
	printf_s("\x1b(0x\x1b(B%s%42s\x1b(0x\x1b(B\n", BG_COLOR, "");
	printf_s("\x1b(0x\x1b(B%s %2s OK  %2s %6c %s Cancel %s\x1b(0x\x1b(B\n", BG_COLOR, state ? COLOR_SELECTED : "", BG_COLOR, ' ', state ? "" : COLOR_SELECTED, "");
	printf_s("\x1b(0x\x1b(B%s%42s\x1b(0x\x1b(B\n", BG_COLOR, "");
	puts("\x1b(0mqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqj\x1b(B\x1b[0;0m");
}

bool AcceptMenu(char* warningMsg) {
	bool acceptState = true;
	do {
		system("cls");
		DrawAcceptMenu(acceptState, warningMsg);
		int input = _getch();
		if (input == KEY_ENTER) {
			return acceptState;
		}
		if (input == KEY_ESCAPE) {
			return false;
		}
		if (input == TWO_BYTE_IN) {
			switch (_getch()) {
			case KEY_RIGHT: { //Arror Right
				acceptState = false;
				break;
			}
			case KEY_LEFT: { //Arrow Left
				acceptState = true;
				break;
			}
			}
		}
	} while (true);
}

// this also only works since i defined the size of our input buffer as a macro else i would have to use a parameter,
bool CheckForInvalidCharackters(char* inputBuffer) {

}


void DrawMenu(MenuData* data) { 
	system("cls");
	puts(BG_COLOR "                                                                            ");
	printf_s("  \x1b(0lqqqqqqqqqqqqqqqqqqqqqqqqqqqqqu \x1b(BConverter\x1b(0 tqqqqqqqqqqqqqqqqqqqqqqqqqqqqk\x1b(B  \n");
	printf_s("  \x1b(0x                                                                      x\x1b(B  \n");
	printf_s("  \x1b(0x    lqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqk lqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqk x\x1b(B  \n");
	printf_s("  \x1b(0x    x%s                              %sx x%s                              %sx x\x1b(B  \n", data->ConvMode ? COLOR_ACTIVE : "", BG_COLOR, data->ConvMode ? "" : COLOR_ACTIVE, BG_COLOR);
	printf_s("  \x1b(0x    x%s\x1b(B%s (N) Dezimal -> Hexadezimal   %s\x1b(0x x\x1b(B%s %s(H) Hexadezimal -> Dezimal   %s\x1b(0x x\x1b(B  \n",
		// Well this part is a bit chaotic.... hope i can improve it later.
		data->ConvMode ? COLOR_ACTIVE : "", data->CurrSelection == CONV_DEZ_HEX ? COLOR_SELECTED : "", COLOR_SELECTED_RES BG_COLOR,
		data->ConvMode ? "" : COLOR_ACTIVE, data->CurrSelection == CONV_HEX_DEZ ? COLOR_SELECTED : "", COLOR_SELECTED_RES BG_COLOR);
	printf_s("  \x1b(0x    x%s                              %sx x%s                              %sx x\x1b(B  \n", data->ConvMode ? COLOR_ACTIVE : "", BG_COLOR, data->ConvMode ? "" : COLOR_ACTIVE, BG_COLOR);
	printf_s("  \x1b(0x    mqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqj mqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqj x\x1b(B  \n");
	printf_s("  \x1b(0x                                                                      x\x1b(B  \n");
	printf_s("  \x1b(0x                                                                      x\x1b(B  \n");
	printf_s("  \x1b(0x                  %51s x\x1b(B  \n", data->ErrorMsg);
	printf_s("  \x1b(0x                  lqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqk x\x1b(B  \n");
	printf_s("  \x1b(0x\x1b(B%s%17s %s\x1b(0x\x1b(B %s%45s \x1b(0x x\x1b(B  \n", 
		data->CurrSelection == INPUT_NUMBER ? COLOR_SELECTED : "", data->ConvMode ? "Input Dezimal" : "Input Hex", COLOR_SELECTED_RES BG_COLOR, data->ConvMode ? "  " : "0x", data->InputBuffer);
	printf_s("  \x1b(0x                  mqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqj x\x1b(B  \n");
	printf_s("  \x1b(0x                                                                      x\x1b(B  \n");
	printf_s("  \x1b(0tqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqu\x1b(B  \n");
	printf_s("  \x1b(0x                                                                      x\x1b(B  \n");
	printf_s("  \x1b(0x                  lqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqk x\x1b(B  \n");
	printf_s("  \x1b(0x\x1b(B%17s \x1b(0x\x1b(B %s%45s \x1b(0x x\x1b(B  \n",
		data->ConvMode ? "Result Hex" : "Result Dezimal", data->ConvMode ? "0x" : "  ", data->ConvResult);
	printf_s("  \x1b(0x                  mqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqj x\x1b(B  \n");
	printf_s("  \x1b(0mqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqj\x1b(B  \n");
	puts("                                                                            ");
	printf_s(" %s(ESC) End%s                                                                  \n", data->CurrSelection == END ? COLOR_SELECTED : "", COLOR_SELECTED_RES BG_COLOR);
	puts("                                                                            \x1b[0;0m\n");
	puts("Enter: Select / Convert   N/H: Swap between Convert Modi     Esc: End Programm");
	puts("\n         ^");
	puts("         |");
	puts("Arrow <--o-->  Keys zum Selectieren");
	puts("         |");
	puts("         v");
}

// return a bool if the new character is allowed in a hex or decimal 
bool ValidSymbole(char c, bool hexAllowed) {
	return (((c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f')) && hexAllowed) || (c >= '0' && c <= '9');
}

bool InputLoop(MenuData* data) {
	for (;;) {
		int input = _getch();
		switch (input) {
		case KEY_ENTER: {
			return true;
		}
		case KEY_ESCAPE: {
			return false;
		}
		case KEY_DELETE: {
			data->InputBuffer[data->BufferPosIdx--] = ' ';
			break;
		}
		default: {
			if (ValidSymbole(input, data->ConvMode) && data->BufferPosIdx < INPUT_BUFFER_SIZE) {
				data->InputBuffer[data->BufferPosIdx++] = input;
				if (data->ConvMode) {
					ConvDezHex(data->InputBuffer, data->ConvResult);
				}
				else {
					ConvHexDez(data->InputBuffer, data->ConvResult);
				}
			}
			else {
				
			}
			break;
		}
		}
		DrawMenu(data);
	}
}
	


void Menu() {

	MenuData data;
	// init Values
  data.ConvMode = true;
	data.ErrorMsg = "";
	data.ConvResult = "";
	for (int i = 0; i < INPUT_BUFFER_SIZE; i++) {
		data.InputBuffer[i]="\0";
	}
	data.BufferPosIdx = 0;
	data.CurrSelection = INPUT_NUMBER;

	for (;;) {
		DrawMenu(&data);
		int inC = _getch();
		int arrK;

		switch (inC) {
		case 'h':
		case 'H': {
			data.ConvMode = false;
			break;
		}
		case 'n':
		case 'N': {
			data.ConvMode = true;
			break;
		}
		case TWO_BYTE_IN: {
			arrK = _getch();
			// Not optimal if we add more Conv modi this will get a bit confusion an unreadable. And its hard coded -.- 
			if (data.CurrSelection == CONV_DEZ_HEX || data.CurrSelection == CONV_HEX_DEZ) {
				if (arrK == KEY_LEFT) {
					data.CurrSelection = CONV_DEZ_HEX;
				}
				if (arrK == KEY_RIGHT) {
					data.CurrSelection = CONV_HEX_DEZ;
				}
				if (arrK == KEY_DOWN) {
					data.CurrSelection = INPUT_NUMBER;
				}
			}
			else {
				if (arrK == KEY_DOWN) {
					data.CurrSelection += 1;
				}
				if (arrK == KEY_UP) {
					data.CurrSelection -= 1;
				}
			}
			break;
		}
		case KEY_ESCAPE: {
			if (AcceptMenu("Wollen sie das Programm beenden?")) {
				puts("\033[0;37mProgramm has been Stopped\033[0;0m");
				return;
			}
			break;
		}
		case KEY_ENTER: {
			switch (data.CurrSelection) {
			case CONV_DEZ_HEX: {
				data.ConvMode = true;
				break;
			}
			case CONV_HEX_DEZ: {
				data.ConvMode = false;
				break;
			}
			case INPUT_NUMBER: {
				InputLoop(&data);
				break;
			}
			case END: {
				if (AcceptMenu("Wollen sie das Programm wirklich beenden?")) {
					puts("\033[0;37mProgramm has been Stopped\033[0;0m");
					return;
				}
				break;
			}
			}
			break;
		}
		}
	}
}

#pragma endregion



int main()
{
	Menu();
}
