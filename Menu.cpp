#include "Menu.h"

Menu menu;

void Menu::DrawWindow()
{
	interfaces.Surface->DrawSetColor(27, 27, 27, 255);

	interfaces.Surface->DrawFilledRect(400, 340, menu.menu_x, menu.menu_y);
}