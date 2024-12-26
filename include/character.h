#ifndef CHARACTER_H
#define CHARACTER_H

#include "common.h"

class Charactor
{
public:
	// Charactor Attributes
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up = { 0.0f, 1.0f, 0.0f };
	glm::vec3 Right;
	glm::vec3 WorldUp;
	// euler Angles
	float Yaw;
	// Charactor options
	float MovementSpeed;
	float MouseSensitivity;
	float Zoom;

	//constructor with vectors: position, front, up, yaw, pitch
	Charactor(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f), 
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f))
		: Position(position), Front(front),
		 WorldUp(up), Yaw(-90.0f), MovementSpeed(2.5f), MouseSensitivity(0.04f), Zoom(45.0f)
	{
		updateCharactorVectors();
	}

	glm::mat4 GetViewMatrix()
	{
		return glm::lookAt(Position, Position + Front, Up);
	}

	void SetMovementSpeed(float speed)
	{
		MovementSpeed = speed;
	}

	void SetMouseSensitivity(float sensitivity)
	{
		MouseSensitivity = sensitivity;
	}


	void move(Movement direction, float deltaTime)
	{
		float velocity = MovementSpeed * deltaTime;
		switch (direction) {
		case FORWARD:
			Position += Front * velocity;
			break;
		case BACKWARD:
			Position -= Front * velocity;
			break;
		case LEFT:
			Position -= Right * velocity;
			break;
		case RIGHT:
			Position += Right * velocity;
			break;
		case UP:
			Position += Up * velocity;
			break;
		case DOWN:
			Position -= Up * velocity;
			break;
		}
	}

	void ProcessMouseMovement(float xoffset, float yoffset)
	{
		xoffset *= MouseSensitivity;
		Yaw += xoffset;
		updateCharactorVectors();
	}

	void zoom(float yoffset)
	{
		Zoom -= (float)yoffset;
		if (Zoom < 1.0f)
			Zoom = 1.0f;
		if (Zoom > 90.0f)
			Zoom = 90.0f;
	}

private:
	void updateCharactorVectors()
	{
		// calculate the new Front vector
		glm::vec3 front;
		front.x = cos(glm::radians(Yaw));
		front.y = .0f;
		front.z = sin(glm::radians(Yaw));
		Front = glm::normalize(front);
		Right = glm::normalize(glm::cross(Front, WorldUp));
		Up = glm::normalize(glm::cross(Right, Front));
	}
};

#endif