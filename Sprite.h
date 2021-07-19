#pragma once
#include "glut.h"

class Sprite
{
public:
	struct Point
	{
		GLfloat x;
		GLfloat y;
	};

	struct Size
	{
		GLfloat width;
		GLfloat height;
	};

	struct Rect
	{
		GLfloat top;
		GLfloat bottom;
		GLfloat left;
		GLfloat right;
	};

	enum SpriteState
	{
		UP,
		DOWN
	};

protected:
	GLuint* m_textures;
	unsigned int m_textureIndex;
	unsigned int m_currentFrame;
	unsigned int m_numberOfFrames;
	GLfloat m_animationDelay;
	GLfloat m_animationElapsed;

	Point m_position;
	Size m_size;
	GLfloat m_velocityX;
	GLfloat m_velocityY;

	bool m_isCollideable;
	bool m_flipHorizontal;
	bool m_flipVertical;
	bool m_isVisible;
	bool m_isActive;
	bool m_useTransparency;
	bool m_isSpriteSheet;
	bool m_isClicked;

	Rect m_collision;
	int m_value;

	float m_radius;
	Point m_center;

public:
	Sprite(const unsigned int p_numberOfTextures);
	~Sprite();

	void Update(const float p_deltaTime);
	void Render();

	const bool AddTexture(const char* p_fileName, const bool p_useTransparency = true);
	const bool SetTextureID(const Sprite* pSprite, const bool p_useTransparency = true);
	const GLuint GetCurrentFrame() { return m_isSpriteSheet ? m_textures[0] : m_textures[m_currentFrame]; }

	void SetPosition(const GLfloat p_x, const GLfloat p_y) { m_position.x = p_x; m_position.y = p_y; }
	void SetPosition(const Point p_position) { m_position = p_position; }
	const Point GetPosition() const { return m_position; }
	const Size GetSize() const { return m_size; }
	void SetFrameSize(const GLfloat p_width, const GLfloat p_height) { m_size.width = p_width; m_size.height = p_height; }
	void SetVelocityX(const GLfloat p_velocityX) { m_velocityX = p_velocityX; }
	void SetVelocityY(const GLfloat p_velocityY) { m_velocityY = p_velocityY; }
	const GLfloat GetVelocityX() const { return m_velocityX; }
	const GLfloat GetVelocityY() const { return m_velocityY; }
	void SetNumberOfFrames(const unsigned int p_frames) { m_numberOfFrames = p_frames; }


	void IsCollideable(const bool p_value) { m_isCollideable = p_value; }
	void FlipHorizontal(const bool p_value) { m_flipHorizontal = p_value; }
	void FlipVertical(const bool p_value) { m_flipVertical = p_value; }
	void IsActive(const bool p_value) { m_isActive = p_value; }
	const bool IsActive() const { return m_isActive; }
	void IsVisible(const bool p_value) { m_isVisible = p_value; }
	const bool IsVisible() const { return m_isVisible; }
	const bool IsCollideable() const { return m_isCollideable; }

	void UseTransparency(const bool p_value) { m_useTransparency = p_value; }

	void Jump(SpriteState p_state);
	void IsClicked(const bool p_value) { m_isClicked = p_value; }
	const bool IsClicked() const { return m_isClicked; }

	const Rect GetCollisionRect() const;
	void SetCollisionRectOffset(const Rect p_rect) { m_collision = p_rect; }

	void SetValue(const int p_value) { m_value = p_value; }
	const int GetValue() const { return m_value; }

	void SetRadius(const GLfloat p_radius) { m_radius = p_radius; }
	const float GetRadius() const { return m_radius; }
	void SetCenter(const Point p_center) { m_center = p_center; }
	const Point GetCenter() const;

	const bool IntersectsCircle(const Sprite* p_sprite) const;

	const bool IntersectsRect(const Sprite* p_sprite) const;
};

