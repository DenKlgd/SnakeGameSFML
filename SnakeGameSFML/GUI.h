#pragma once
#include <SFML/Graphics.hpp>
using namespace sf;


class GUI_Button
{
private:

protected:
	void (*command_ptr)() = nullptr;
	bool isActive = false;
	short id = -1;

public:
	GUI_Button() {}
	void setID(short id)
	{
		if (id <= 0)
			return;
		this->id = id;
	}
	virtual ~GUI_Button() {}
};


class GUI_Text_Button : public GUI_Button
{
private:
	RectangleShape rectangle;
	Text text;
	Font font;

	void changeButtonColor(Color text_color, Color btn_color)
	{
		rectangle.setFillColor(Color(btn_color));
		text.setFillColor(Color(text_color));
	}

	bool isButtonPressed(Event& event, Vector2i& mousePos)
	{
		if (event.type == Event::MouseButtonPressed && event.key.code == Mouse::Left)
			if (rectangle.getGlobalBounds().contains(mousePos.x, mousePos.y))
			{
				changeButtonColor(Color::White, Color::Red);
				//outputToConsole("PRESSED");
				isActive = true;
				return true;
			}
		return false;
	}

	short isButtonReleased(Event& event, Vector2i& mousePos)
	{
		if (event.type == Event::MouseButtonReleased && event.key.code == Mouse::Left)
		{
			if (rectangle.getGlobalBounds().contains(mousePos.x, mousePos.y))
			{
				if (isActive)
				{
					changeButtonColor(Color::Red, Color::White);
					//outputToConsole("RELEASED");
					if (command_ptr != nullptr)
						command_ptr();
					return id;
				}
			}
			else
			{
				isActive = false;
				changeButtonColor(Color::Red, Color::White);
			}
		}
		return 0;
	}

public:
	GUI_Text_Button(){}

	GUI_Text_Button(String btn_text, int font_size, Font& font, Color text_color, float size[], Color btn_color)
	{
		init(btn_text, font_size, font, text_color, size, btn_color);
	}

	void init(String btn_text, int font_size, Font& font, Color text_color, float size[], Color btn_color)
	{
		rectangle.setSize(Vector2f(size[0], size[1]));
		rectangle.setFillColor(Color(btn_color));
		this->font = font;
		text.setString(btn_text);
		text.setCharacterSize(font_size);
		text.setFont(font);
		text.setFillColor(Color(text_color));
	}

	void setText(String newText)
	{
		text.setString(newText);
	}

	void setPosition(float x, float y)
	{
		rectangle.setPosition(x, y);
		float position_X = rectangle.getPosition().x + rectangle.getGlobalBounds().width / 2 - text.getGlobalBounds().width / 2;
		float position_Y = rectangle.getPosition().y + rectangle.getGlobalBounds().height / 2 - text.getGlobalBounds().height;
		text.setPosition(position_X, position_Y);
	}

	void resize(float x, float y)
	{
		rectangle.setSize(sf::Vector2f(x, y));
	}

	void assignProcedure(void (*command)())
	{
		command_ptr = command;
	}

	short onClick(Event& event, Vector2i& mousePos)
	{
		isButtonPressed(event, mousePos);
		return isButtonReleased(event, mousePos);
	}

	void drawButton(RenderWindow& window)
	{
		window.draw(rectangle);
		window.draw(text);
	}
};


class GUI_Picture_Button : public GUI_Button
{
private:
	Sprite sprite;

	void changeButtonColor(Color color)
	{
		sprite.setColor(color);
	}

	bool isButtonPressed(Event& event, Vector2i& mousePos)
	{
		if (event.type == Event::MouseButtonPressed && event.key.code == Mouse::Left)
			if (sprite.getGlobalBounds().contains(mousePos.x, mousePos.y))
			{
				changeButtonColor(Color::Red);
				isActive = true;
				return true;
			}
		return false;
	}

	short isButtonReleased(Event& event, Vector2i& mousePos)
	{
		if (event.type == Event::MouseButtonReleased && event.key.code == Mouse::Left)
		{
			if (sprite.getGlobalBounds().contains(mousePos.x, mousePos.y))
			{
				if (isActive)
				{
					changeButtonColor(Color::White);
					if (command_ptr != nullptr)
						command_ptr();
					return id;
				}
			}
			else
			{
				isActive = false;
				changeButtonColor(Color::White);
			}
		}
		return 0;
	}
public:
	GUI_Picture_Button() {}

	GUI_Picture_Button(float x, float y)
	{
		sprite.setPosition(x, y);
	}

	void setTexture(Texture& texture)
	{
		sprite.setTexture(texture);
	}

	void setPosition(float x, float y)
	{
		sprite.setPosition(x, y);
	}

	void assignProcedure(void (*command)())
	{
		command_ptr = command;
	}

	short onClick(Event& event, Vector2i& mousePos)
	{
		isButtonPressed(event, mousePos);
		return isButtonReleased(event, mousePos);
	}

	void drawButton(RenderWindow& window)
	{
		window.draw(sprite);
	}
};
