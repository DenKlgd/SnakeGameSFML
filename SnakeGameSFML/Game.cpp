#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <Windows.h>
#include "resource.h"
#include "GUI.h"
#define minX 22
#define minY 16

using namespace sf;

enum STATE {
	UNDEFINED,
	GAME,
	MENU,
	OPTIONS,
	CONTROLS,
	WIN,
};

int size = 32, offset = 50;
int x = 22, y = 16;
int width = x * size, height = y * size;
int score = 0, set_score = 0, scoreToBeat;
bool playDeathAnim = false;
STATE gameState = MENU;
SYSTEMTIME sysTime;

struct Velocity
{
	short x, y, x_prev, y_prev;
} v;

struct Snake
{
	short x, y;
} s[102];

int SnakeMax = sizeof(s) / sizeof(s[0]);

struct Fruit
{
	short x, y;
} f;

RenderWindow window(VideoMode(width, height + offset), "SNAKE GAME", sf::Style::Titlebar | sf::Style::Close);
Event event;
Vector2i mousePos;
Font font;
Sprite head, shadow, white, red, green;
SoundBuffer coinSoundBuffer, deathSoundBuffer, buttonSoundBuffer;
Sound sound;
Text scoreText, winText;
RectangleShape rectangle;
Color text_color = Color::Red;
Color btn_color = Color::White;
GUI_Text_Button* button_ptr_array[4];
int arr_size = sizeof(button_ptr_array) / sizeof(button_ptr_array[0]);
float rect_size[] = { width / 6, 50 };
float position[] = { width / 2 - rect_size[0] / 2, height / 2 - rect_size[1] * arr_size / 2 - rect_size[1] / 2 };
short buttonID = 0;

Text* labels = nullptr, *outputValues = nullptr;
Texture* minusTexture = nullptr, *plusTexture = nullptr;
GUI_Picture_Button* plusButtons = nullptr, *minusButtons = nullptr;
GUI_Text_Button* textButtons = nullptr;
const int optionsButtonCount = 3;
short controlsCount = 0;

unsigned int random(unsigned int seed)
{
	seed = (seed * 73129 + 95121) % 100000;
	return seed;
}

void Input()
{
	if (Event::KeyPressed) { v.x_prev = v.x; v.y_prev = v.y; }
	if (Keyboard::isKeyPressed(Keyboard::Escape)) { gameState = MENU; score = set_score; playDeathAnim = false; }
	if (Keyboard::isKeyPressed(Keyboard::Left)) { v.x = -1; v.y = 0; }
	if (Keyboard::isKeyPressed(Keyboard::Right)) { v.x = 1; v.y = 0; }
	if (Keyboard::isKeyPressed(Keyboard::Up)) { v.x = 0; v.y = -1; }
	if (Keyboard::isKeyPressed(Keyboard::Down)) { v.x = 0; v.y = 1; }
	if (v.x == v.x_prev * -1 && v.y == v.y_prev * -1)
	{
		v.x *= -1;
		v.y *= -1;
	}
}

void Logic()
{
	if (playDeathAnim)
	{
		if (score > 0)
			--score;
		else
			playDeathAnim = false;
		sleep(sf::milliseconds(80));
		return;
	}

	for (int i = score + 1; i > 0; i--)
	{
		s[i].x = s[i - 1].x;
		s[i].y = s[i - 1].y;
	}

	s[0].x += v.x; s[0].y += v.y;

	if (s[0].x == x)
		s[0].x = 0;
	else if (s[0].x == -1)
		s[0].x = x - 1;
	else if (s[0].y == y)
		s[0].y = 0;
	else if (s[0].y == -1)
		s[0].y = y - 1;

	if (s[0].x == f.x && s[0].y == f.y)
	{
		sound.setBuffer(coinSoundBuffer);
		sound.play();
		score++;
		bool randSuccess = false;
		while (!randSuccess)
		{
			for (unsigned int i = 0; i < score + 2; i++)
			{
				if (f.x == s[i].x && f.y == s[i].y)
				{
					GetLocalTime(&sysTime);
					f.x = random(sysTime.wYear + sysTime.wDay + sysTime.wHour + sysTime.wMinute + sysTime.wSecond + sysTime.wMilliseconds) % x;
					f.y = random(sysTime.wYear + sysTime.wDay + sysTime.wHour + sysTime.wMinute + sysTime.wSecond + sysTime.wMilliseconds) % y;
					break;
				}
				randSuccess = true;
			}
		}
		s[score + 1].x = s[score].x;
		s[score + 1].y = s[score].y;
	}

	for (int i = 1; i <= score + 1; i++)
	{
		if (s[0].x == s[i].x && s[0].y == s[i].y)
		{
			sound.setBuffer(deathSoundBuffer);
			sound.play();
			playDeathAnim = true;
		}
	}
}

void InitSnake()
{
	for (int i = 0; i < score + 2; i++)
	{
		s[i].x = x / 2 - i;
		s[i].y = y / 2;
	}
	v.x = 1; v.y = 0;
}

void RenderGame()
{
	window.clear();
	scoreText.setString("Score: " + std::to_string(score));
	window.draw(rectangle);
	window.draw(scoreText);

	for (int j = 0; j < y; j++)
		for (int i = 0; i < x; i++)
		{
			white.setPosition(i * size, j * size + offset);
			window.draw(white);
		}

	head.setPosition(s[0].x * size, s[0].y * size + offset);
	shadow.setPosition(s[0].x * size, s[0].y * size + offset);

	if (v.y == 1)
	{
		head.setRotation(0.);
		head.move(0, 0);
	}
	else if (v.y == -1)
	{
		head.setRotation(180.);
		head.move(size, size);
	}
	else if (v.x == 1)
	{
		head.setRotation(270.);
		head.move(0, size);
	}
	else if (v.x == -1)
	{
		head.setRotation(90.);
		head.move(size, 0);
	}

	window.draw(head);
	window.draw(shadow);

	for (int i = 1; i < score + 2; i++)
	{
		red.setPosition(s[i].x * size, s[i].y * size + offset);
		window.draw(red);
	}
	green.setPosition(f.x * size, f.y * size + offset); window.draw(green);

	window.display();
}

void Start_Game()
{
	gameState = GAME;
	GetLocalTime(&sysTime);
	f.x = random(sysTime.wYear + sysTime.wDay + sysTime.wHour + sysTime.wMinute + sysTime.wSecond + sysTime.wMilliseconds) % x;
	f.y = random(sysTime.wYear + sysTime.wDay + sysTime.wHour + sysTime.wMinute + sysTime.wSecond + sysTime.wMilliseconds) % y;
	InitSnake();
}

void Quit()
{
	window.close();
}

void setTextAttr(Text &text)
{
	text.setCharacterSize(32);
	text.setFont(font);
	text.setFillColor(Color(255, 0, 0));
}

void resizeGUI()
{
	width = x * size, height = y * size;
	float rect_size[2] = { width / 6, 50 };
	const float position_X = width / 2 + size / 2;
	const float position_Y = height / 2 - rect_size[1] * optionsButtonCount / 2 - rect_size[1] / 2;
	window.setSize(sf::Vector2u(width, height + offset));
	textButtons[0].setPosition(width / 2 - rect_size[0] / 2, position_Y + optionsButtonCount * rect_size[1] * 1.5);
	textButtons[1].setPosition(width / 2 - rect_size[0] / 2, position_Y + rect_size[1] * 1.5 + optionsButtonCount * rect_size[1] * 1.5);

	for (int i = 0; i < optionsButtonCount; i++)
	{
		labels[i].setPosition(width / 2 - labels[i].getGlobalBounds().width, position_Y + i * rect_size[1] * 1.5);
		outputValues[i].setPosition(position_X + size * 1.75, position_Y + i * rect_size[1] * 1.5);
		plusButtons[i].setPosition(position_X, position_Y + i * rect_size[1] * 1.5 + 3);
		minusButtons[i].setPosition(position_X + size * 3.5, position_Y + i * rect_size[1] * 1.5 + 3);
	}
}

void DestructOptionsMenu()
{
	delete[] labels;
	delete[] outputValues;
	delete[] plusButtons;
	delete[] minusButtons;
	delete[] textButtons;
	delete plusTexture;
	delete minusTexture;

	labels = nullptr;
	outputValues = nullptr;
	plusButtons = nullptr;
	minusButtons = nullptr;
	plusTexture = nullptr;
	minusTexture = nullptr;
	textButtons = nullptr;

	gameState = MENU;
}

void InitOptionsMenu()
{
	float rect_size[2] = { width / 6, 50 };
	const float position_X = width / 2 + size / 2;
	const float position_Y = height / 2 - rect_size[1] * optionsButtonCount / 2 - rect_size[1] / 2;

	minusTexture = new Texture; plusTexture = new Texture;
	minusTexture->loadFromFile("images/minus.png");
	plusTexture->loadFromFile("images/plus.png");

	labels = new Text[optionsButtonCount];
	outputValues = new Text[optionsButtonCount];
	plusButtons = new GUI_Picture_Button[optionsButtonCount];
	minusButtons = new GUI_Picture_Button[optionsButtonCount];

	labels[0].setString("GAMEFIELD WIDTH:"); 
	labels[1].setString("GAMEFIELD HEIGHT:"); 
	labels[2].setString("DIFFICULTY:");
	
	outputValues[0].setString(std::to_string(x));
	outputValues[1].setString(std::to_string(y));
	outputValues[2].setString(std::to_string(scoreToBeat));

	for (int i = 0; i < optionsButtonCount; i++)
	{
		labels[i].setCharacterSize(32);
		labels[i].setFont(font);
		labels[i].setFillColor(Color(180, 32, 32));
		labels[i].setLetterSpacing(3);
		labels[i].setPosition(width / 2 - labels[i].getGlobalBounds().width, position_Y + i * rect_size[1] * 1.5);

		setTextAttr(outputValues[i]);
		outputValues[i].setPosition(position_X + size * 1.75, position_Y + i * rect_size[1] * 1.5 );
		
		plusButtons[i].setPosition(position_X, position_Y + i * rect_size[1] * 1.5 + 3);
		minusButtons[i].setPosition(position_X + size * 3.5, position_Y + i * rect_size[1] * 1.5 + 3);
		plusButtons[i].setTexture(*plusTexture);
		minusButtons[i].setTexture(*minusTexture);
		plusButtons[i].setID(i + 10);
		minusButtons[i].setID(i + 20);
	}

	textButtons = new GUI_Text_Button[2];

	textButtons[0].init("DEFAULT", 32, font, Color::Red, rect_size, Color::White);
	textButtons[0].setPosition(width / 2 - rect_size[0] / 2, position_Y + optionsButtonCount * rect_size[1] * 1.5);
	textButtons[0].setID(200);

	textButtons[1].init("GO BACK", 32, font, Color::Red, rect_size, Color::White);
	textButtons[1].setPosition(width / 2 - rect_size[0]/2, position_Y + rect_size[1] * 1.5 + optionsButtonCount * rect_size[1] * 1.5);
	textButtons[1].setID(100);

	gameState = OPTIONS;
}

void optionsMenuProcedure(const short buttonID)
{
	switch (buttonID)
	{
	case 100:
		position[0] = { width / 2 - rect_size[0] / 2 };
		position[1] = { height / 2 - rect_size[1] * arr_size / 2 - rect_size[1] / 2 };
		for (int i = 0; i < arr_size; i++)
			button_ptr_array[i]->setPosition(position[0], position[1] + i * rect_size[1] * 1.5);
		winText.setPosition(width / 2 - 145, height / 2 - 10);
		scoreText.setPosition(width / 2 - 54, 8);
		rectangle.setSize(Vector2f(width, offset));
		DestructOptionsMenu();
		break;
	case 200:
		x = 22; y = 16;
		scoreToBeat = 50;
		outputValues[0].setString(std::to_string(x));
		outputValues[1].setString(std::to_string(y));
		outputValues[2].setString(std::to_string(scoreToBeat));
		resizeGUI();
		break;
	case 10:
		if (x >= sf::VideoMode::getDesktopMode().width / size)
			break;
		++x; 
		outputValues[0].setString(std::to_string(x)); 
		resizeGUI();
		break;
	case 20:
		if (x <= minX)
			break;
		--x;
		outputValues[0].setString(std::to_string(x));
		resizeGUI();
		break;
	case 11:
		if (y >= sf::VideoMode::getDesktopMode().height / size - 2)
			break;
		++y;
		outputValues[1].setString(std::to_string(y));
		resizeGUI();
		break;
	case 21:
		if (y <= minY)
			break;
		--y;
		outputValues[1].setString(std::to_string(y));
		resizeGUI();
		break;
	case 12:
		if (scoreToBeat >= SnakeMax - 2)
			break;
		++scoreToBeat;
		outputValues[2].setString(std::to_string(scoreToBeat));
		break;
	case 22:
		if (scoreToBeat <= 2)
			break;
		--scoreToBeat;
		outputValues[2].setString(std::to_string(scoreToBeat));
		break;
	default:
		break;
	}
}

void getOptionsClickedButtonID()
{
	for (int i = 0; i < optionsButtonCount; i++)
	{
		buttonID = plusButtons[i].onClick(event, mousePos);
		if (buttonID > 0)
			return;
		buttonID = minusButtons[i].onClick(event, mousePos);
		if (buttonID > 0)
			return;
	}
	for (int i = 0; i < 2; i++)
	{
		buttonID = textButtons[i].onClick(event, mousePos);
		if (buttonID > 0)
			return;
	}
	return;
}

void destructControlsMenu()
{
	delete[] labels;
	labels = nullptr;
	delete textButtons;
	textButtons = nullptr;
	gameState = MENU;
}

void initControlsMenu()
{
	String controlLabels[] = {"Up:", "Down:", "Left:", "Right:", "Return to menu:"};
	String controls[] = {"[ Arrow UP ]", "[ Arrow DOWN ]", "[ Arrow LEFT ]", "[ Arrow RIGHT ]", "[ ESC ]" };
	controlsCount = sizeof(controlLabels) / sizeof(controlLabels[0]);
	const float position_X = width / 2 - 20;
	const float position_Y = height / 2 - 40 * controlsCount / 2 - 32 / 2;
	labels = new Text[controlsCount * 2];

	for (short i = 0; i < controlsCount; i++)
	{
		setTextAttr(labels[i]);
		labels[i].setFillColor(Color(180, 32, 32));
		labels[i].setString(std::move(controlLabels[i]));
		labels[i].setPosition(width / 2 - labels[i].getGlobalBounds().width, position_Y + i * 32 * 1.4);
		
		setTextAttr(labels[i + controlsCount]);
		labels[i + controlsCount].setFillColor(Color(128, 48, 48));
		labels[i + controlsCount].setString(std::move(controls[i]));
		labels[i + controlsCount].setPosition(width / 2 + 20, position_Y + i * 32 * 1.4);
	}

	textButtons = new GUI_Text_Button;
	textButtons->init("GO BACK", 32, font, Color::Red, rect_size, Color::White);
	textButtons->setPosition(width / 2 - rect_size[0] / 2, position_Y + controlsCount * 32 * 1.5);
	textButtons->assignProcedure(destructControlsMenu);

	gameState = CONTROLS;
}


int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, PSTR pCmdArgs, int nCmdShow)
{
	HICON hIcon = LoadIconA(hInst, MAKEINTRESOURCEA(IDI_ICON1));
	SetClassLongA(GetActiveWindow(), GCL_HICON, (LONG)hIcon);
	ZeroMemory(&sysTime, sizeof(sysTime));

	String fontName("wiguru-13.ttf");
	font.loadFromFile(fontName);

	GUI_Text_Button startButton("START", 32, font, text_color, rect_size, btn_color);
	GUI_Text_Button optionsButton("OPTIONS", 32, font, text_color, rect_size, btn_color);
	GUI_Text_Button controlsButton("CONTROLS", 32, font, text_color, rect_size, btn_color);
	GUI_Text_Button exitButton("QUIT", 32, font, text_color, rect_size, btn_color);

	button_ptr_array[0] = &startButton;
	button_ptr_array[1] = &optionsButton;
	button_ptr_array[2] = &controlsButton;
	button_ptr_array[3] = &exitButton;

	for (int i = 0; i < arr_size; i++)
		button_ptr_array[i]->setPosition(position[0], position[1] + i * rect_size[1] * 1.5);

	startButton.assignProcedure(Start_Game);
	optionsButton.assignProcedure(InitOptionsMenu);
	controlsButton.assignProcedure(initControlsMenu);
	exitButton.assignProcedure(Quit);

	Texture headTexture, shadowMask, t1, t2, t3;
	headTexture.loadFromFile("images/head.png");
	shadowMask.loadFromFile("images/shadow_mask.png");
	t1.loadFromFile("images/white.png");
	t2.loadFromFile("images/red.png");
	t3.loadFromFile("images/green.png");

	head.setTexture(headTexture);
	shadow.setTexture(shadowMask);
	white.setTexture(t1);
	red.setTexture(t2);
	green.setTexture(t3);

	scoreText.setFont(font); winText.setFont(font);
	scoreText.setCharacterSize(32); winText.setCharacterSize(32);
	scoreText.setFillColor(Color(255, 0, 0)); winText.setFillColor(Color(255, 0, 0));
	scoreText.setPosition(width/2-54,8);

	winText.setString("You've won the game of Snake!");
	winText.setPosition(width / 2 - 145, height / 2 - 10);

	rectangle.setSize(Vector2f(width, offset));
	rectangle.setFillColor(Color(192, 192, 192));

	coinSoundBuffer.loadFromFile("sounds/coin.wav");
	deathSoundBuffer.loadFromFile("sounds/death.wav");
	buttonSoundBuffer.loadFromFile("sounds/button.wav");
	sound.setBuffer(buttonSoundBuffer);
	sound.setVolume(100.);

	Clock clock;
	float timer = 0, delay = 0.1;

	InitSnake();
	GetLocalTime(&sysTime);

	f.x = random(sysTime.wYear + sysTime.wDay + sysTime.wHour + sysTime.wMinute + sysTime.wSecond + sysTime.wMilliseconds) % x;
	f.y = random(sysTime.wYear + sysTime.wDay + sysTime.wHour + sysTime.wMinute + sysTime.wSecond + sysTime.wMilliseconds) % y;

	scoreToBeat = 50;

	while (window.isOpen())
	{
		while (window.pollEvent(event))
		{
			buttonID = 0;
			if (event.type == Event::Closed)
				window.close();
			if (event.type == Event::Resized)
			{
				FloatRect view(0, 0, event.size.width, event.size.height);
				window.setView(View(view));
			}
			switch (gameState)
			{
			case MENU:
				if (event.type == Event::MouseButtonPressed || event.type == Event::MouseButtonReleased)
					for (int i = 0; i < arr_size; i++)
						if (buttonID = button_ptr_array[i]->onClick(event, mousePos))
							break;
				break;
			case OPTIONS:
				if (event.type == Event::MouseButtonPressed || event.type == Event::MouseButtonReleased)
				{
					getOptionsClickedButtonID();
					optionsMenuProcedure(buttonID);
				}
				break;
			case CONTROLS:
				buttonID = textButtons->onClick(event, mousePos);
				break;
			case WIN:
				if (event.type == Event::KeyPressed)
				{
					score = set_score;
					gameState = MENU;
					InitSnake();
				}
				break;
			case GAME:
				break;
			default:
				break;
			}
			if (buttonID)
			{
				sound.setBuffer(buttonSoundBuffer);
				sound.play();
			}
		}

		switch (gameState)
		{
		case MENU:
			window.clear(Color(192, 192, 192));
			startButton.drawButton(window);
			optionsButton.drawButton(window);
			controlsButton.drawButton(window);
			exitButton.drawButton(window);
			window.display();
			mousePos = Mouse::getPosition(window);
			break;

		case OPTIONS:
			window.clear(Color(192, 192, 192));
			for (int i = 0; i < optionsButtonCount; i++)
			{
				window.draw(labels[i]);
				plusButtons[i].drawButton(window);
				minusButtons[i].drawButton(window);
				window.draw(outputValues[i]);
			}
			textButtons[0].drawButton(window);
			textButtons[1].drawButton(window);
			window.display();
			mousePos = Mouse::getPosition(window);
			break;

		case CONTROLS:
			window.clear(Color(192, 192, 192));
			for (short i = 0; i < controlsCount * 2; i++)
			{
				window.draw(labels[i]);
			}
			textButtons->drawButton(window);
			window.display();
			mousePos = Mouse::getPosition(window);
			break;

		case GAME:
			timer += clock.getElapsedTime().asSeconds();
			clock.restart();
			Input();
			if (timer > delay)
			{
				timer = 0;
				Logic();
				RenderGame();
				if (score == scoreToBeat)
				{
					gameState = WIN;
					timer = 0;
				}
			}
			break;

		case WIN:
			window.clear(Color(192, 192, 192));
			window.draw(winText);
			window.display();
			sleep(sf::milliseconds(450));
			break;

		default:
			break;
		}
	}
	return 0;
}
