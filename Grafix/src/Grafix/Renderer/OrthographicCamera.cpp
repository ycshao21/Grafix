#include "pch.h"
#include "OrthographicCamera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Grafix
{
    OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top, float zNear, float zFar)
        : m_ProjectionMatrix(glm::ortho(left, right, bottom, top, zNear, zFar)), m_ViewMatrix(1.0f)
    {
        RecalculateMatrices();
    }

    void OrthographicCamera::RecalculateMatrices()
    {
        glm::mat4 translate = glm::translate(glm::mat4(1.0f), m_Position);
        glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation), glm::vec3(0.0f, 0.0f, 1.0f));

        glm::mat4 transform = translate * rotate;

        m_ViewMatrix = glm::inverse(transform);
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
    }
}

