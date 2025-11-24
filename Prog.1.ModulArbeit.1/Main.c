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
#define COLOR_SELECTED     "\033[5;34m"
#define COLOR_ACTIVE       "\033[1;107m"
#define BG_COLOR           "\x1b[47m\033[1;30m"

//Limit of our input since Number with more then 20 digits are already larger, then what can be stored in a long long so we dont have to store those.  
#define INPUT_BUFFER_SIZE   21
#define LIMIT_DEZIMAL_INPUT 19
#define LIMIT_HEX_INPUT     16
#define ERROR_MESSAGE_LIMIT 5

enum STATE {
	DEZIMAL_TO_HEX_SEL = 0,
	HEX_TO_DEZIMAL_SEL,
	INPUT_NUMBER,
	END ,
};
enum ConvMode {
	DEZIMAL_TO_HEX = 0,
	HEX_TO_DEZIMAL,
};

typedef struct  {
	int ErrorFlags;
	/*
	* 1 << 0 = Invalid Input
	* 1 << 1 = Invalid exceed limits
	* 1 << 2 = Invalid char in input String: can be trigger after switching from hex->dec to dec->Hex
	* 1 << 3 = Invalid ConvMode
	*/
	char* ConvResult;
	char* InputBuffer;
	int BufferPosIdx;
	int ConvResulSize;
	enum ConvMode ActivConvMode;
	enum STATE CurrSelection;
}MenuData;

#pragma region Converter

#pragma region Helper

void ClearArray(char* buf, int* size) {
	for (int i = 0; i < (*size); i++) {
		buf[i] = '\0';
	}
	(*size) = 0;
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
	ClearArray(o_Res, o_ResSize);
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
	ClearArray(o_Res, o_ResSize);
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


// Draw the "are you sure!" menu to check if the user knows what he is doing
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


void GetErrorMessages(int* o_NumOfMsg, char** o_Messages, int errorFlags) {
	if (!errorFlags) {
		return;
	}
	*o_NumOfMsg = 0;
	if (errorFlags & 1 << 0) {
		o_Messages[(*o_NumOfMsg)++] = "[Fehler] Fehlerhaften Charakter eingegeben!";
	}
	if (errorFlags & 1 << 1) {
		o_Messages[(*o_NumOfMsg)++] = "[Fehler] Ueberschreitung des eingabe Limits!";
	}
	if (errorFlags & 1 << 2) {
		o_Messages[(*o_NumOfMsg)++] = "[Fehler] Ungueltiges Symbole in der Eingabe!";
	}	
	if (errorFlags & 1 << 3) {
		o_Messages[(*o_NumOfMsg)++] = "[Fehler] Ungueltiger Convertierungs Modus!";
	}
}

void DrawMenu(MenuData* displayData) { 

	char* errorMessages[ERROR_MESSAGE_LIMIT];
	int numOfErros = 0;
	GetErrorMessages(&numOfErros, errorMessages, displayData->ErrorFlags);
	
	//to make the draw function more readable, will convert i convmode Enum to a bool.
	// true = Dec, false = Hex
	bool isDecMode = displayData->ActivConvMode == DEZIMAL_TO_HEX;

	system("cls");
	puts(BG_COLOR "                                                                            ");
	printf_s("  \x1b(0lqqqqqqqqqqqqqqqqqqqqqqqqqqqqqu \x1b(BConverter\x1b(0 tqqqqqqqqqqqqqqqqqqqqqqqqqqqqk\x1b(B  \n");
	printf_s("  \x1b(0x                                                                      x\x1b(B  \n");
	printf_s("  \x1b(0x    lqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqk lqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqk x\x1b(B  \n");
	printf_s("  \x1b(0x    x%s                              %sx x%s                              %sx x\x1b(B  \n", isDecMode ? COLOR_ACTIVE : "", BG_COLOR, isDecMode ? "" : COLOR_ACTIVE, BG_COLOR);
	printf_s("  \x1b(0x    x%s\x1b(B%s (N) Dezimal -> Hexadezimal   %s\x1b(0x x\x1b(B%s %s(H) Hexadezimal -> Dezimal   %s\x1b(0x x\x1b(B  \n",
		//TODO Well this part is a bit chaotic.... hope i can improve it later. Only a temporary solution.
		isDecMode ? COLOR_ACTIVE : "", displayData->CurrSelection == DEZIMAL_TO_HEX_SEL ? COLOR_SELECTED : "", COLOR_SELECTED_RES BG_COLOR,
		isDecMode ? "" : COLOR_ACTIVE, displayData->CurrSelection == HEX_TO_DEZIMAL_SEL ? COLOR_SELECTED : "", COLOR_SELECTED_RES BG_COLOR);
	printf_s("  \x1b(0x    x%s                              %sx x%s                              %sx x\x1b(B  \n", isDecMode ? COLOR_ACTIVE : "", BG_COLOR, isDecMode ? "" : COLOR_ACTIVE, BG_COLOR);
	printf_s("  \x1b(0x    mqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqj mqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqj x\x1b(B  \n");
	printf_s("  \x1b(0x                                                                      x\x1b(B  \n");
	printf_s("  \x1b(0x                                                                      x\x1b(B  \n");
	printf_s("  \x1b(0x           \x1b(B %23s\x1b(0                                   x\x1b(B  \n", isDecMode ? "0-9" : "0-9, a-f, A-F");
	printf_s("  \x1b(0x                  lqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqk x\x1b(B  \n");
	printf_s("  \x1b(0x\x1b(B%s%17s %s\x1b(0x\x1b(B %s%.*s\x1b[5m_\x1b[25m%*s                               \x1b(0x x\x1b(B  \n", 
		displayData->CurrSelection == INPUT_NUMBER ? COLOR_SELECTED : "",
		isDecMode ? "Input Dezimal" : "Input Hex",
		COLOR_SELECTED_RES BG_COLOR,
		isDecMode ? "  " : "0x",
		displayData->BufferPosIdx + 1,
		displayData->InputBuffer,
		INPUT_BUFFER_SIZE - displayData->BufferPosIdx,
		BG_COLOR);
	printf_s("  \x1b(0x                  mqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqj x\x1b(B  \n");
	//printf_s("  \x1b(0x                  \x1b(B\x1b[0;47;31m%45s\x1b(0      \x1b[0;0m%s x\x1b(B  \n", displayData->ErrorMsg, BG_COLOR);
	printf_s("  \x1b(0tqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqu\x1b(B  \n");
	printf_s("  \x1b(0x                                                                      x\x1b(B  \n");
	printf_s("  \x1b(0x                  lqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqk x\x1b(B  \n");
	printf_s("  \x1b(0x\x1b(B%17s \x1b(0x\x1b(B %s%s%*s \x1b(0x x\x1b(B  \n",         
		isDecMode ? "Result Hex" : "Result Dezimal", isDecMode ? "0x" : "  ", displayData->ConvResult, 64 - displayData->ConvResulSize, COLOR_SELECTED_RES BG_COLOR); // "%.*s" the dot means we only want to print so many char 
	printf_s("  \x1b(0x                  mqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqj x\x1b(B  \n");
	printf_s("  \x1b(0mqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqj\x1b(B  \n");
	printf_s("                                                                            %s\n", COLOR_SELECTED_RES);
	printf_s("%s %s(ESC) End                                                                  %s\n", BG_COLOR, displayData->CurrSelection == END ? COLOR_SELECTED : "", COLOR_SELECTED_RES);
	printf_s("%s                                                                            \x1b[0;0m\n", BG_COLOR);
	puts("Enter: Auswahl / Konvertiert   N/H: Wechsel zwischen Konvertier Modi     Esc: Beende Programm");
	puts("\n         ^");
	puts("         |");
	puts("Arrow <--o-->  Keys zum Selektieren");
	puts("         |");
	puts("         v");
}
	
// since we allocated memory for our Input and Output buffer we have to free them at the end.
void CleanUp(MenuData* displayData) {
	free(displayData->InputBuffer);
	free(displayData->ConvResult);
	puts("\033[0;37mProgramm has been Stopped\033[0;0m");
}

int ArrowKeyHandling(int key, enum STATE* io_State) {
	int retCode = 1;
	switch (key) {
	case KEY_UP: {
		*io_State = max(max((*io_State), HEX_TO_DEZIMAL_SEL) - 1, HEX_TO_DEZIMAL_SEL);
		break; }
	case KEY_DOWN: {
		*io_State = min(max((*io_State) + 1, INPUT_NUMBER), END);
		break; }
	case KEY_RIGHT: { 
		if ((*io_State) == DEZIMAL_TO_HEX_SEL) {
			*io_State = min((*io_State) + 1, HEX_TO_DEZIMAL_SEL);
		}
		break; }
	case KEY_LEFT: {
		if ((*io_State) == HEX_TO_DEZIMAL_SEL) {
			*io_State = max((*io_State) - 1, DEZIMAL_TO_HEX);
		}
		break;
	}
	default: {
		retCode = -1;
		break;
	}
	}
	return retCode;
}

void Menu() {

	// Initialize values using designated initializers (allowed since C99)
	MenuData displayData = {
		.ErrorFlags = 0,
		.ConvResult = calloc(INPUT_BUFFER_SIZE * 2 , sizeof(char)),
		.ConvResulSize = 0,
		.BufferPosIdx = 0,
		.CurrSelection = INPUT_NUMBER,
		.ActivConvMode = DEZIMAL_TO_HEX,
	  .InputBuffer = calloc(INPUT_BUFFER_SIZE, sizeof(char))
	};
	bool showMenu = true;
	// The main Loop for our Men, everything that we do in the menu is happing in here.
	while(showMenu) {
		DrawMenu(&displayData);
		int input = _getch();
		int inSecoundary;
		switch (input) {
		case KEY_ENTER: {
			switch (displayData.CurrSelection) {
				case HEX_TO_DEZIMAL_SEL: { 
					displayData.ActivConvMode = HEX_TO_DEZIMAL;
					break; }
				case DEZIMAL_TO_HEX_SEL: { 
					displayData.ActivConvMode = DEZIMAL_TO_HEX;
					break; }
				default: {
					// we just ignore this.
					break;
				}
		  }
			if (displayData.CurrSelection == END) {
				showMenu = false;
				CleanUp(&displayData);
			}
		}
		case KEY_DELETE: {
			if (displayData.BufferPosIdx >= 0) {
				displayData.InputBuffer[displayData.BufferPosIdx] = '\0';
				displayData.BufferPosIdx = max(displayData.BufferPosIdx - 1, 0);
			}
			break;
		}
		case KEY_ESCAPE: { 
			showMenu = false;
			CleanUp(&displayData);
			break; }
		case TWO_BYTE_IN: {
			inSecoundary = _getch();
			int retCode = ArrowKeyHandling(inSecoundary, &displayData.CurrSelection);
			if (retCode != 1) {
				displayData.ErrorFlags |= 1 << 0; // set error flag for invalid input character
			}
			break;
		}
	  default: {
			if (ValidSymbole(input, displayData.ActivConvMode == HEX_TO_DEZIMAL)) {
				// we use a switch since we could in theory expand the capabilities of the converter to binary or octa
				int writePos = displayData.BufferPosIdx;
				switch (displayData.ActivConvMode) {
				case HEX_TO_DEZIMAL: {
					if (writePos < LIMIT_HEX_INPUT) {
						displayData.InputBuffer[displayData.BufferPosIdx++] = input;
					}
					break; }
				case DEZIMAL_TO_HEX: {
					if (writePos < LIMIT_DEZIMAL_INPUT) {
						displayData.InputBuffer[displayData.BufferPosIdx++] = input;
					}
					break; }
				default: { 
					displayData.ErrorFlags |= 1 << 4;
					break; }
				 }
			}
			else {
			 displayData.ErrorFlags |= 1 << 0;
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
