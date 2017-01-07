

#include "Camera.h"
#include <iostream>

#include <string>

#include "glm\glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace common {


		Camera::Camera(glm::vec3 position , glm::vec3 up, GLfloat yaw , GLfloat pitch ) : forward(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVTY), Zoom(ZOOM)

		{
			this->position = position;
			this->WorldUp = up;
			this->Yaw = yaw;
			this->Pitch = pitch;
			this->updateCameraVectors();
		}

		Camera::Camera(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX, GLfloat upY, GLfloat upZ, GLfloat yaw, GLfloat pitch) : forward(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVTY), Zoom(ZOOM)
		{
			this->position = glm::vec3(posX, posY, posZ);
			this->WorldUp = glm::vec3(upX, upY, upZ);
			this->Yaw = yaw;
			this->Pitch = pitch;
			this->updateCameraVectors();

		}

		glm::mat4 Camera::GetViewMatrix()
		{
			return glm::lookAt(this->position, this->position + this->forward, this->up); 
		}


		// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
		void Camera::ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime)
		{
			GLfloat velocity = this->MovementSpeed * deltaTime;
			if (direction == FORWARD)
				this->position += this->forward * velocity;
			if (direction == BACKWARD)
				this->position -= this->forward * velocity;
			if (direction == LEFT)
				this->position -= this->right * velocity;
			if (direction == RIGHT)
				this->position += this->right * velocity;
		}

		// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
		void Camera::ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch )
		{
			xoffset *= this->MouseSensitivity;
			yoffset *= this->MouseSensitivity;

			this->Yaw += xoffset;
			this->Pitch += yoffset;

			// Make sure that when pitch is out of bounds, screen doesn't get flipped
			if (constrainPitch)
			{
				if (this->Pitch > 89.0f)
					this->Pitch = 89.0f;
				if (this->Pitch < -89.0f)
					this->Pitch = -89.0f;
			}

			// Update Front, Right and Up Vectors using the updated Eular angles
			this->updateCameraVectors();
		}

		// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
		void Camera::ProcessMouseScroll(GLfloat yoffset)
		{
			if (this->Zoom >= 1.0f && this->Zoom <= 45.0f)
				this->Zoom -= yoffset;
			if (this->Zoom <= 1.0f)
				this->Zoom = 1.0f;
			if (this->Zoom >= 45.0f)
				this->Zoom = 45.0f;
		}

	
		void Camera::updateCameraVectors()
		{
			// Calculate the new Front vector
			glm::vec3 front;
			front.x = cos(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
			front.y = sin(glm::radians(this->Pitch));
			front.z = sin(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
			this->forward = glm::normalize(front);
			// Also re-calculate the Right and Up vector
			this->right = glm::normalize(glm::cross(this->forward, this->WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
			this->up = glm::normalize(glm::cross(this->right, this->forward));
		}

	


}

