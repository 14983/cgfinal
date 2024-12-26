#ifndef CAMERA_H
#define CAMERA_H

#include "common.h"
#include "character.h"

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.04f;
const float ZOOM = 45.0f;

class Camera
{
public:
	// camera Attributes
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up = { 0.0f, 1.0f, 0.0f };
	glm::vec3 Right;
	glm::vec3 WorldUp;
	// euler Angles
	float Yaw = YAW;
	float Pitch = PITCH;
	// camera options
	float MovementSpeed;
	float MouseSensitivity;
	float Zoom;
	bool Object;

	//constructor with vectors: position, front, up, yaw, pitch
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f), bool Object = false, glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH)
		: Position(position), Front(front), 
		 WorldUp(up), Yaw(yaw), Pitch(pitch), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM), Object(Object)
	{
		updateCameraVectors();
	}

	glm::mat4 GetViewMatrix()
	{
		return glm::lookAt(Position, Position + Front, Up);
	}

	void zoom(float yoffset)
	{
		Zoom -= (float)yoffset;
		if (Zoom < 1.0f)
			Zoom = 1.0f;
		if (Zoom > 90.0f)
			Zoom = 90.0f;
	}

void target(const Charactor& target)
{
    this->Front = glm::vec3(cos(glm::radians(this -> Pitch))) * target.Front + glm::vec3(sin(glm::radians(this -> Pitch))) * glm::vec3(0.0, 1.0, 0.0);
	this->Position = target.Position - glm::vec3(7.0) * (this->Front);
}

	void updatePitch(float dY) {
		static const float MAX_PITCH = 30.0;
		Pitch += dY * SENSITIVITY;
		if (Pitch < -MAX_PITCH) Pitch = -MAX_PITCH;
		if (Pitch > MAX_PITCH) Pitch = MAX_PITCH;
	}
private:
	void updateCameraVectors()
	{
		// calculate the new Front vector
		glm::vec3 front;
		front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		front.y = sin(glm::radians(Pitch));
		front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		Front = glm::normalize(front);
		Right = glm::normalize(glm::cross(Front, WorldUp));
		Up = glm::normalize(glm::cross(Right, Front));
	}
};

#endif