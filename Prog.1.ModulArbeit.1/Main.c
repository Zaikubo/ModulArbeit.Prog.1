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

//Limit of our input since Number with more then 20 digits are already larger, then what can be stored in a long long so we dont have to store those.  
#define INPUT_BUFFER_SIZE 21

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
	//the same as Conv Mode only this time we save if the user is currently in Input mode (want to input something in the input box).
	// Normaly you would never want to save it in the main Struct, but for this light weight interface its more convenient to do it anyways.,
	bool ActivInput;

	char* ErrorMsg;
	char* ConvResult;
	char* InputBuffer;
	int BufferPosIdx;
	int ConvResulSize;
	enum STATE CurrSelection;
}MenuData;
#pragma region Converter

#pragma region Helper

void clearArray(char* buf, int size) {
	for (int i = 0; i < size; i++) {
		buf[i] = "\0";
	}
}
// return a bool if the new character is allowed in a hex or decimal 
bool ValidSymbole(char c, bool hexAllowed) {
	return (((c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f')) && hexAllowed) || (c >= '0' && c <= '9');
}

bool CheckForInvalidCharackters(char* inputBuffer, int size, bool hex) {
	for (int i = 0; i < size; i++) {
		if (!ValidSymbole(inputBuffer[i], hex)) {
			return false;
		}
	}
	return true;
}

#pragma endregion

void ConvDezHex(char* dez, int size, char* o_Res, int* o_ResSize) {
	unsigned long long inputValue;
	char tempRes[30] = " ";

	int pos = 0;
	clearArray(o_Res, *o_ResSize);
	if (sscanf_s(dez, "%llu", &inputValue) && size > 0) {
		if (inputValue == 0) {
			*o_ResSize = 1;
			o_Res[0] = '0';
			return;
		}

		for (; inputValue != 0 && pos < 30; pos++) {
			char c = inputValue % 16;
			if (c < 10) {
				tempRes[pos] = c + '0';
			}
			else {
				tempRes[pos] = c - 10 + 'A';
			}
			inputValue /= 16;
		}
		pos--; // to throw away the '\0' at the end
		for (*o_ResSize = 0; pos >= 0; (*o_ResSize)++, pos--) {
			o_Res[*o_ResSize] = tempRes[pos];
		}
		o_Res[(*o_ResSize)] = '\0';
	}
	else {
		o_Res[0] = 'N';
		o_Res[1] = 'A';
		o_Res[2] = 'N';
		o_Res[3] = '\0';
		*o_ResSize = 3;
	}
}
void ConvHexDez(char* hex, int size, char* o_Res, int* o_ResSize) {
	clearArray(o_Res, *o_ResSize);
	unsigned long long result = 0;
	if (CheckForInvalidCharackters(hex, size, true)) {
		for (int i = 0; i < size; i++) {
			int num = 1;
			if (hex[i] >= '0' && hex[i] <= '9') {
				num = hex[i] - '0';
			}
			if (hex[i] >= 'A' && hex[i] <= 'F') {
				num = hex[i] - 'A' + 10;
			}
			if (hex[i] >= 'a' && hex[i] <= 'f') {
				num = hex[i] - 'a' + 10;
			}
			result = (result * 16) + num;
		}
		unsigned long long reCopy = result;
		(*o_ResSize) = 0;
		char tempStringRes[24];
		for (;reCopy != 0;) {
			tempStringRes[(*o_ResSize)++] = (reCopy % 10) + '0';
			reCopy /= 10;
		}
		//revers the result string
		int k = 0;
		for (int i = *o_ResSize - 1; i >= 0; i--, k++) {
			o_Res[k] = tempStringRes[i];
		}
	}
	else {
		o_Res[0] = 'N';
		o_Res[1] = 'A';
		o_Res[2] = 'N';
		o_Res[3] = '\0';
		*o_ResSize = 3;
	}
}

#pragma endregion

#pragma region  Menu



void DrawAcceptMenu(bool state, char* warningMsg) {
	printf_s("%s\x1b(0lqqqqqqqqqqqqq\x1b(B Do you want to Continue ? \x1b(0qqqqqqqqqqqqqk\x1b(B\n",BG_COLOR);
	printf_s("\x1b(0x\x1b(B%45s        \x1b(0x\x1b(B\n", warningMsg);
	printf_s("\x1b(0x%53sx\x1b(B\n", "");
	printf_s("\x1b(0x    \x1b(B%s   OK    %s                         %s   Cancel   %s   \x1b(0x\x1b(B\n", state ? COLOR_SELECTED : "", COLOR_SELECTED_RES BG_COLOR, state ? "" : COLOR_SELECTED, COLOR_SELECTED_RES BG_COLOR);
	printf_s("\x1b(0x\x1b(B%s%53s\x1b(0x\x1b(B\n", BG_COLOR, "");
	puts("\x1b(0mqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqj\x1b(B\x1b[0;0m");
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



void DrawMenu(MenuData* data) { 
	system("cls");
	puts(BG_COLOR "                                                                            ");
	printf_s("  \x1b(0lqqqqqqqqqqqqqqqqqqqqqqqqqqqqqu \x1b(BConverter\x1b(0 tqqqqqqqqqqqqqqqqqqqqqqqqqqqqk\x1b(B  \n");
	printf_s("  \x1b(0x                                                                      x\x1b(B  \n");
	printf_s("  \x1b(0x    lqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqk lqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqk x\x1b(B  \n");
	printf_s("  \x1b(0x    x%s                              %sx x%s                              %sx x\x1b(B  \n", data->ConvMode ? COLOR_ACTIVE : "", BG_COLOR, data->ConvMode ? "" : COLOR_ACTIVE, BG_COLOR);
	printf_s("  \x1b(0x    x%s\x1b(B%s (N) Dezimal -> Hexadezimal   %s\x1b(0x x\x1b(B%s %s(H) Hexadezimal -> Dezimal   %s\x1b(0x x\x1b(B  \n",
		//TODO Well this part is a bit chaotic.... hope i can improve it later. Only a temporary solution.
		data->ConvMode ? COLOR_ACTIVE : "", data->CurrSelection == CONV_DEZ_HEX ? COLOR_SELECTED : "", COLOR_SELECTED_RES BG_COLOR,
		data->ConvMode ? "" : COLOR_ACTIVE, data->CurrSelection == CONV_HEX_DEZ ? COLOR_SELECTED : "", COLOR_SELECTED_RES BG_COLOR);
	printf_s("  \x1b(0x    x%s                              %sx x%s                              %sx x\x1b(B  \n", data->ConvMode ? COLOR_ACTIVE : "", BG_COLOR, data->ConvMode ? "" : COLOR_ACTIVE, BG_COLOR);
	printf_s("  \x1b(0x    mqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqj mqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqj x\x1b(B  \n");
	printf_s("  \x1b(0x                                                                      x\x1b(B  \n");
	printf_s("  \x1b(0x                                                                      x\x1b(B  \n");
	printf_s("  \x1b(0x           \x1b(B %23s\x1b(0                                   x\x1b(B %2d \n", data->ConvMode ? "0-9" : "0-9, a-f, A-F", data->BufferPosIdx);
	printf_s("  \x1b(0x                  lqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqk x\x1b(B  \n");
	printf_s("  \x1b(0x\x1b(B%s%17s %s\x1b(0x\x1b(B%s %s%.*s %*s \x1b(0x x\x1b(B  \n", 
		data->CurrSelection == INPUT_NUMBER ? COLOR_SELECTED : "",
		data->ConvMode ? "Input Dezimal" : "Input Hex",
		COLOR_SELECTED_RES BG_COLOR, data->ConvMode ? "  " : "0x",
		data->ActivInput ? "\033[43m" : "", 
		data->BufferPosIdx + 1, 
		data->InputBuffer,
		63 - data->BufferPosIdx, 
		COLOR_SELECTED_RES BG_COLOR);
	printf_s("  \x1b(0x                  mqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqj x\x1b(B  \n");
	printf_s("  \x1b(0x                  \x1b(B\x1b[0;47;31m%45s\x1b(0      \x1b[0;0m%s x\x1b(B  \n", data->ErrorMsg, BG_COLOR);
	printf_s("  \x1b(0tqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqu\x1b(B  \n");
	printf_s("  \x1b(0x                                                                      x\x1b(B  \n");
	printf_s("  \x1b(0x                  lqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqk x\x1b(B  \n");
	printf_s("  \x1b(0x\x1b(B%17s \x1b(0x\x1b(B %s%s%*s \x1b(0x x\x1b(B  \n",         
		data->ConvMode ? "Result Hex" : "Result Dezimal", data->ConvMode ? "0x" : "  ", data->ConvResult, 64 - data->ConvResulSize, COLOR_SELECTED_RES BG_COLOR); // "%.*s" the dot means we only want to print so many char 
	printf_s("  \x1b(0x                  mqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqj x\x1b(B  \n");
	printf_s("  \x1b(0mqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqj\x1b(B  \n");
	printf_s("                                                                            %s\n", COLOR_SELECTED_RES);
	printf_s("%s %s(ESC) End                                                                  %s\n", BG_COLOR, data->CurrSelection == END ? COLOR_SELECTED : "", COLOR_SELECTED_RES);
	printf_s("%s                                                                            \x1b[0;0m\n", BG_COLOR);
	puts("Enter: Select / Convert   N/H: Swap between Convert Modi     Esc: End Programm");
	puts("\n         ^");
	puts("         |");
	puts("Arrow <--o-->  Keys zum Selectieren");
	puts("         |");
	puts("         v");
	if (CheckForInvalidCharackters(data->InputBuffer, data->BufferPosIdx, !data->ConvMode)) {
		data->ErrorMsg = "";
	}
}



bool InputLoop(MenuData* data) {
	DrawMenu(data);

	for (;;) {
		int input = _getch();
		switch (input) {
		case KEY_ESCAPE: {
			return false;
		}
		case KEY_DELETE: {
			if (data->BufferPosIdx >= 0) {
				data->BufferPosIdx -= data->BufferPosIdx == 0 ? 0 : 1;
				data->InputBuffer[data->BufferPosIdx] = '\0';
			}
			if (data->ConvMode) {
				ConvDezHex(data->InputBuffer, data->BufferPosIdx, data->ConvResult, &data->ConvResulSize);
			}
			else {
				ConvHexDez(data->InputBuffer, data->BufferPosIdx, data->ConvResult, &data->ConvResulSize);
			}
			break;
		}
		default: {
			if (ValidSymbole(input, !data->ConvMode) && ((data->BufferPosIdx < 19 && data->ConvMode) || ((data->BufferPosIdx < 16 && !data->ConvMode)) )) {
				data->InputBuffer[data->BufferPosIdx++] = input;
				if (data->ConvMode ) {
					ConvDezHex(data->InputBuffer, data->BufferPosIdx, data->ConvResult, &data->ConvResulSize);
				}
				else {
					ConvHexDez(data->InputBuffer, data->BufferPosIdx, data->ConvResult, &data->ConvResulSize);
				}
			}
			else {
				if ((data->BufferPosIdx >= 19 && data->ConvMode) || (data->BufferPosIdx >= 16 && !data->ConvMode)) {
					data->ErrorMsg = "[Fehler]: Convertierungs Limit erreicht";
				}
				else {
					data->ErrorMsg = "[Fehler]: Invalide Eingabe";
				}
			}
			break;
		}
		}
		DrawMenu(data);
	}
}
	
void CleanUp(MenuData* data) {
	free(data->InputBuffer);
	free(data->ConvResult);
	free(data->ErrorMsg);
	puts("\033[0;37mProgramm has been Stopped\033[0;0m");
}

void Menu() {

	// Initialize values using designated initializers (allowed since C99)
	MenuData data = {
		.ActivInput = false,
		.ConvMode = true,
		.ErrorMsg = calloc(40 , sizeof(char)),
		.ConvResult = calloc(INPUT_BUFFER_SIZE * 2 , sizeof(char)),
		.ConvResulSize = 0,
		.BufferPosIdx = 0,
		.CurrSelection = INPUT_NUMBER,
	  .InputBuffer = calloc(INPUT_BUFFER_SIZE, sizeof(char))
	};

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
			if (!CheckForInvalidCharackters(data.InputBuffer, data.BufferPosIdx, false)) {
				data.ErrorMsg = "[Fehler]: Invalid Input";
			}
			break;
		}
		case TWO_BYTE_IN: {
			arrK = _getch();
			// Not optimal if we add more Conv modi, this will get a bit more confusion and unreadable. And its hard coded -.- (LEN)
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
				CleanUp(&data);
				return;
			}
			break;
		}
		case KEY_ENTER: {
			switch (data.CurrSelection) {
			case CONV_DEZ_HEX: {
				data.ConvMode = true;
				if (!CheckForInvalidCharackters(data.InputBuffer, data.BufferPosIdx, false)) {
					data.ErrorMsg = "[Fehler]: Invalid Input";
				}
				break;
			}
			case CONV_HEX_DEZ: {
				data.ConvMode = false;
				break;
			}
			case INPUT_NUMBER: {
				data.ActivInput = true;
			  InputLoop(&data);
			  data.ActivInput = false;
				break;
			}
			case END: {
				if (AcceptMenu("Wollen sie das Programm wirklich beenden?")) {
					CleanUp(&data);
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
