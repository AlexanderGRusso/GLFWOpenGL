#pragma once

#include <vector>

#define GLEW_STATIC
#include <gl/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum Camera_Movement 
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

const GLfloat YAW = -90.0F;
const GLfloat PITCH = 0.0F;
const GLfloat SPEED = 6.0F;
const GLfloat SENSITIVITY = 0.25F;
const GLfloat ZOOM = 45.0F;

class Camera 
{
public: 
	Camera(glm::vec3 position = glm::vec3(0.0F, 0.0F, 0.0F), glm::vec3 up = glm::vec3(0.0F, 1.0F, 0.0F), GLfloat yaw = YAW, GLfloat pitch = PITCH): front(glm::vec3(0.0F, 0.0F, -1.0F)), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM)
	{
		this->position = position;
		this->worldUp = up;
		this->yaw = yaw;
		this->pitch = pitch;
		this->updateCameraVectors();
	}

	Camera(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX, GLfloat upY, GLfloat upZ, GLfloat yaw, GLfloat pitch): front(glm::vec3(0.0F, 0.0F, -1.0F)), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM)
	{
		this->position = glm::vec3(posX, posY, posZ);
		this->worldUp = glm::vec3(upX, upY, upZ);
		this->yaw = yaw;
		this->pitch = pitch;
		this->updateCameraVectors();
	}

	glm::mat4 getViewMatrix() 
	{
		return glm::lookAt(this->position, this->position + this->front, this->worldUp);
	}

	void processKeyboard(Camera_Movement direction, GLfloat deltaTime) 
	{
		GLfloat velocity = this->movementSpeed * deltaTime;
		if (FORWARD == direction)
		{
			this->position += this->front * velocity;
		}
		if (BACKWARD == direction)
		{
			this->position -= this->front * velocity;
		}
		if (LEFT == direction)
		{
			this->position -= this->right * velocity;
		}
		if (RIGHT == direction)
		{
			this->position += this->right * velocity;
		}
	}

	void processMouseMovement(GLfloat xOffset, GLfloat yOffset, GLboolean constrainPitch = true) 
	{
		xOffset *= this->mouseSensitivity;
		yOffset *= this->mouseSensitivity;

		this->yaw += xOffset;
		this->pitch += yOffset;

		if (constrainPitch)
		{
			if (this->pitch > 89.0F)
				this->pitch = 89.0F;
			
			if (this->pitch < -89.0F)
				this->pitch = -89.0F;
		}

		this->updateCameraVectors();
	}

	void processMouseScroll(GLfloat yOffset) 
	{
		if (this->zoom >= 1.0F && this->zoom <= 45.0F) 
		{
			this->zoom -= yOffset;
		}
		if (this->zoom < 1.0F)
		{
			this->zoom = 1.0F;
		}
		if (this->zoom > 45.0F)
		{
			this->zoom = 45.0F;
		}
	}

	GLfloat getZoom() 
	{
		return this->zoom;
	}

	glm::vec3 getPosition()
	{
		return this->position;
	}

private:
	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 worldUp;

	GLfloat yaw;
	GLfloat pitch;

	GLfloat movementSpeed;
	GLfloat mouseSensitivity;
	GLfloat zoom;

	void updateCameraVectors() 
	{
		glm::vec3 front;
		front.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
		front.y = sin(glm::radians(this->pitch));
		front.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));

		this->front = glm::normalize(front);
		this->right = glm::normalize(glm::cross(this->front, this->worldUp));
		this->up = glm::normalize(glm::cross(this->right, this->front));
	}
};