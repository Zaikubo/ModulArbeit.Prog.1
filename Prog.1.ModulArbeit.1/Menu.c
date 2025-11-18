#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "Menu.h"

#define COLOR_SELECTED "\033[1;30m"
#define COLOR_ACTIVE "\033[1;107m"
#define BG_COLOR "\x1b[47m\033[1;30m"
#define BGBG_COLOR "\x1b[0m"

#pragma region Alternative Menu
#define INPUT_BUFFER_SIZE 47

// this also only works since i defined the size of our input buffer as a macro else i would have to use a parameter,
bool CheckForInvalidCharackters(char* inputBuffer) {

}

/// <summary>
/// Draw the Menu and clears the Console before redrawing
/// </summary>
/// <param name="input">Currently stored input number</param>
/// <param name="convResult">The conversion Result</param>
/// <param name="errorMsg">Show an error in the Input String</param>
/// <param name="activeMode">our current Conversation Modi</param>
void DrawAlternativMenu(char* input, char* convResult, char* errorMsg, bool activeMode) {
	system("cls");
	puts(BG_COLOR "                                                                            ");
	printf_s("  \x1b(0lqqqqqqqqqqqqqqqqqqqqqqqqqqqqqu \x1b(BConverter\x1b(0 tqqqqqqqqqqqqqqqqqqqqqqqqqqqqk\x1b(B  \n"                );
	printf_s("  \x1b(0x                                                                      x\x1b(B  \n"                            );
	printf_s("  \x1b(0x    lqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqk lqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqk x\x1b(B  \n"                            );
	printf_s("  \x1b(0x    x%s                              %sx x%s                              %sx x\x1b(B  \n"               , activeMode ? COLOR_ACTIVE : "", BG_COLOR, activeMode ? "" : COLOR_ACTIVE, BG_COLOR);
	printf_s("  \x1b(0x    x%s\x1b(B (N) Dezimal -> Hexadezimal   %s\x1b(0x x\x1b(B%s (H) Hexadezimal -> Dezimal   %s\x1b(0x x\x1b(B  \n", activeMode ? COLOR_ACTIVE : "", BG_COLOR, activeMode ? "" : COLOR_ACTIVE, BG_COLOR);
	printf_s("  \x1b(0x    x%s                              %sx x%s                              %sx x\x1b(B  \n"               , activeMode ? COLOR_ACTIVE : "", BG_COLOR, activeMode ? "" : COLOR_ACTIVE, BG_COLOR);
	printf_s("  \x1b(0x    mqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqj mqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqj x\x1b(B  \n"                        );
	printf_s("  \x1b(0x                                                                      x\x1b(B  \n"                        );
	printf_s("  \x1b(0x                                                                      x\x1b(B  \n"                        );
	printf_s("  \x1b(0x                  %51s x\x1b(B  \n"                                                                      , errorMsg );
	printf_s("  \x1b(0x                  lqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqk x\x1b(B  \n"                        );
	printf_s("  \x1b(0x\x1b(B%17s \x1b(0x\x1b(B %47s \x1b(0x x\x1b(B  \n", "Input Dezimal"                                       , input);			                         
	printf_s("  \x1b(0x                  mqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqj x\x1b(B  \n"                        );
	printf_s("  \x1b(0x                                                                      x\x1b(B  \n"                        );
	printf_s("  \x1b(0tqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqu\x1b(B  \n"                        );
	printf_s("  \x1b(0x                                                                      x\x1b(B  \n"                        );
	printf_s("  \x1b(0x                  lqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqk x\x1b(B  \n"                        );
	printf_s("  \x1b(0x\x1b(B%17s \x1b(0x\x1b(B %47s \x1b(0x x\x1b(B  \n", "Result Hex"                                          , convResult);		                         
	printf_s("  \x1b(0x                  mqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqj x\x1b(B  \n"                        );
	printf_s("  \x1b(0mqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqj\x1b(B  \n"                        );
	puts(    "                                                                            ");
	puts(    " (ESC) End                                                                  ");
	puts(    "                                                                            \x1b[0;0m\n");
}

// return a bool if the new character is allowed in a hex or decimal 
bool ValidSymbole(char c, bool hexAllowed) {
	return (((c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f')) && hexAllowed) || (c >= '0' && c <= '9');
}

void AlternativMenu() {
	//Well this part is a bit wacky since, I'm misusing a boal as a State, but since we only have 2 modi it works and makes later check simpler to write.
	// true = Decimal -> Hex, false = Hex -> Decimal
	bool convMode = true;
	char* errorMsg = "";
	char inputBuffer[INPUT_BUFFER_SIZE] = "";
	int bufferPosIdx = 0;

	for (;;) {
		DrawAlternativMenu(inputBuffer, "", "",convMode);
		char inC = _getch();
		if (ValidSymbole(inC, !convMode) && (bufferPosIdx < INPUT_BUFFER_SIZE - 1)) {
			inputBuffer[bufferPosIdx++] = inC;
		}
		else
		{
			switch (inC) {
			case 'h':
			case 'H': { 
				convMode = false;
				break; }
			case 'n':
			case 'N': {
				convMode = true;
				break;
			}
			case 224: {
				break;
			}
			case KEY_ENTER: {
				break;
			}
			case KEY_DELETE: {
				inputBuffer[--bufferPosIdx] = '\0';
				break;
			}
			case KEY_ESCAPE:{ 
				puts("\033[0;37mProgramm has been Stopped\033[0;0m");
				return;
				break; }
			default:{ 
				break; }
			}
		}
	}

}
#pragma endregion

void DrawAcceptMenu(bool state, char* warningMsg) {
	puts("\x1b(0lqqqqqqqqqqqqq\x1b(B Do you want to Continue ? \x1b(0qqqqqqqqqqqqqk\x1b(B");
	printf_s("\x1b(0x\x1b(B%s%42s\x1b(0x\x1b(B\n", BG_COLOR, BGBG_COLOR);
	printf_s("\x1b(0x\x1b(B%s %2s OK  %2s %6c %s Cancel %s\x1b(0x\x1b(B\n", BG_COLOR, state ? COLOR_SELECTED : BGBG_COLOR, BG_COLOR, ' ', state ? "" : COLOR_SELECTED, BGBG_COLOR);
	printf_s("\x1b(0x\x1b(B%s%42s\x1b(0x\x1b(B\n", BG_COLOR, BGBG_COLOR);
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
		if (input == 224) {
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

