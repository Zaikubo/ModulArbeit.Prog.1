#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "Menu.h"


#define COLOR_SELECTED "\033[1;42m"
#define BG_COLOR "\x1b[44m "
#define BGBG_COLOR "\033[0m"

const char* MenuOptions[] = {"1. Dezimal -> Hexadezimal","2. Hexadezimal -> Dezimal","3. Abbruch"};
void DrawMenu(int selection) {

	for (int i = 0; i < (sizeof(MenuOptions) / sizeof(char*)); i++) {
		printf_s("%s %42s\n", BG_COLOR, BGBG_COLOR);
		printf_s("%s %5c %50s %5c %s%s\n", selection == '1' + i ? COLOR_SELECTED : BG_COLOR, ' ', MenuOptions[i], ' ', BG_COLOR, BGBG_COLOR);
		printf_s("%s %42s\n", BG_COLOR, BGBG_COLOR);
	}
}

const int MIN_OPTION = '1';
const int MAX_OPTION = '3';

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


	printf_s("%s%42s\n", BG_COLOR, BGBG_COLOR);
	printf_s("%s%5c OK  %s %6c %s Cancel %s\n",  state ? COLOR_SELECTED : BGBG_COLOR, ' ', BG_COLOR, ' ', state ? "" : COLOR_SELECTED, BGBG_COLOR);
	printf_s("%s%42s\n", BG_COLOR, BGBG_COLOR);

}



bool AcceptMenu() {
	do {
		system("cls");
	  bool acceptState = true;
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

