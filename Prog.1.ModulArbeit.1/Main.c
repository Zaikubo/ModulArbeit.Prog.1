#include <stdio.h>
#include <stdbool.h>
#include "Menu.h"
#include <limits.h>
#include "string.h"

void ConvDezHex(int dez, char* o_Res) {
}
void ConvHexDez(int hex, char* o_Res) {

}

int main()
{
	
	bool run = true;
	do {
		switch (Menu()) {
		case '1': {
			ConvDezHex(-1, NULL);
			break;
		}
		case '2': {
			ConvHexDez(-1, NULL);
			break;
		}
		case '3': {
			if (AcceptMenu()) {
				run = false;
			}
			break;
		}
		} 
	} while (run);
	puts("Programm has stops succesfuly");
}
