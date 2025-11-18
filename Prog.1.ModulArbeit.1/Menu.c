#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "Menu.h"


#define COLOR_SELECTED "\033[0;107m\033[1;31m"
#define BG_COLOR "\x1b[47m\033[1;30m  "
#define BGBG_COLOR "\x1b[0m"

const char* MenuOptions[] = {"1. Dezimal -> Hexadezimal","2. Hexadezimal -> Dezimal","3. Abbruch"};
void DrawMenu(int selection) {

	printf_s("\x1b(0lqqqqqqqqqqqqqqqqqqqq\x1b(B Main Menu \x1b(0qqqqqqqqqqqqqqqqqqk\x1b(B\n");
	for (int i = 0; i < (sizeof(MenuOptions) / sizeof(char*)); i++) {
		printf_s("C %s %48s \x1b(0x\x1b(B\n", BG_COLOR, BGBG_COLOR);
		printf_s("\x1b(0x\x1b(B %s %10s %30s %*s %s \x1b(0x\x1b(B\n", BG_COLOR, selection == '1' + i ? COLOR_SELECTED : BG_COLOR, MenuOptions[i], selection == '1' + i ? 23 : 21, BG_COLOR, BGBG_COLOR);
		printf_s("\x1b(0x\x1b(B %s %48s \x1b(0x\x1b(B\n", BG_COLOR, BGBG_COLOR);
	}
	printf_s("\x1b(0mqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqj\x1b(B\x1b[0;0m");
}

void DrawConverterMenu(char* Input, char* Output) {
	printf_s("Input : ");
	printf_s("Result: ");
}
void ConverterMenu() {
	char buffer[100];
	char* buffPos = buffer;
	for (;;) {
		buffPos = _getch();
		buffPos++;
		buffPos = '\0';
		DrawConverterMenu(buffer, ' ');
	}
}

const int MIN_OPTION = '1';
const int MAX_OPTION = '3';

void DrawAlternativMenu(char* input) {
	system("cls");
	printf_s("\x1b(0lqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq\x1b(BConverter\x1b(0qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqk\x1b(B\n");
	printf_s("\x1b(0x                                                                      x\x1b(B\n");
	printf_s("\x1b(0x    lqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqk lqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqk x\x1b(B\n");
	printf_s("\x1b(0x    x                              x x                              x x\x1b(B\n");
	printf_s("\x1b(0x    x\x1b(B  1. Dezimal -> Hexadezimal   \x1b(0x x\x1b(B  2. Hexadezimal -> Dezimal   \x1b(0x x\x1b(B\n", MenuOptions[0],MenuOptions[2]);
	printf_s("\x1b(0x    x                              x x                              x x\x1b(B\n");
	printf_s("\x1b(0x    mqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqj mqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqj x\x1b(B\n");
	printf_s("\x1b(0x                                                                      x\x1b(B\n");
	printf_s("\x1b(0x                                                                      x\x1b(B\n");
	printf_s("\x1b(0x                                                                      x\x1b(B\n");
	printf_s("\x1b(0x              lqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqk     x\x1b(B\n");
	printf_s("\x1b(0x\x1b(B%s \x1b(0x\x1b(B %47s \x1b(0x     x\x1b(B\n", "Input Dezimal", input);
	printf_s("\x1b(0x              mqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqj     x\x1b(B\n");
	printf_s("\x1b(0x                                                    \x1b(0x\x1b(B\n");

	printf_s("\x1b(0qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq\x1b(B\x1b[0;0m\n");
	printf_s("\x1b(0x\x1b(B                                                     \x1b(0x\x1b(B\n");

	printf_s("\x1b(0x\x1b(B         \x1b(0lqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqk\x1b                \x1b(0x\x1b(B\n");
	printf_s("\x1b(0x\x1b(B     %s    \x1b(0x\x1b(B            %s                        \x1b(0x\x1b(B       \x1b(0x\x1b(B", "Input Dezimal\n", input);
	printf_s("\x1b(0x\x1b(B         \x1b(0mqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqj\x1b                \x1b(0x\x1b(B\n");
	printf_s("\x1b(0mqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqj\x1b(B\x1b[0;0m\n");
}

bool ValidSymbole(char c) {
	return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9');
}

void AlternativMenu() {
	char buffer[46] = "";
	*buffer = '\0';
	buffer[45] = '\0'; // End of Input Buffer;
	char* currPo = buffer;
	for (;;) {
		DrawAlternativMenu(buffer);
		char inC = _getch();
		if (ValidSymbole(inC) && (currPo )) {
			*currPo = inC;
			currPo++;
			*currPo = "\0";
		}
	}

}

int Menu() {
	int selectedOption = '1';
	for (;;) {
		DrawMenu(selectedOption);

		int in = _getch();
		if (in == KEY_ENTER) {
			return selectedOption;
		}
		system("cls");

		int i = 0;
		switch (in) {
		case '1':
		case '2':
		case '3':
			return in;
			// Since the Arrow key are 2 byte we first have to read the first byte and then evaulate the secound 
		case 224:
			i = _getch();
			if (i == KEY_UP) {
				selectedOption -= selectedOption - 1 < MIN_OPTION ? 0 : 1; //arrow up
			}
			else if (i == KEY_DOWN) {
				selectedOption += selectedOption + 1 > MAX_OPTION ? 0 : 1;// arrow down	
			}
			break;
		default:
			fprintf(stderr, "%d is Not a valid Selection\n", in);
		}
	}
}

void DrawAcceptMenu(bool state) {
	puts("\x1b(0lqqqqqqqqqqqqq\x1b(B Do you want to Continue ? \x1b(0qqqqqqqqqqqqqk\x1b(B");
	printf_s("\x1b(0x\x1b(B%s%42s\x1b(0x\x1b(B\n", BG_COLOR, BGBG_COLOR);
	printf_s("\x1b(0x\x1b(B%s %2s OK  %2s %6c %s Cancel %s\x1b(0x\x1b(B\n", BG_COLOR, state ? COLOR_SELECTED : BGBG_COLOR, BG_COLOR, ' ', state ? "" : COLOR_SELECTED, BGBG_COLOR);
	printf_s("\x1b(0x\x1b(B%s%42s\x1b(0x\x1b(B\n", BG_COLOR, BGBG_COLOR);
	puts("\x1b(0mqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqj\x1b(B\x1b[0;0m");
}



bool AcceptMenu() {
	bool acceptState = true;
	do {
		system("cls");
		DrawAcceptMenu(acceptState);
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

