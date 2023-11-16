#pragma once

#include <glm/glm.hpp>

namespace Grafix
{
    class OrthographicCamera
    {
    public:
        OrthographicCamera(float left, float right, float bottom, float top, float zNear = -1.0f, float zFar = 1.0f);

        inline const glm::vec3& GetPosition() const { return m_Position; }
        inline void SetPosition(const glm::vec3& position) { m_Position = position; RecalculateMatrices(); }

        // NOTE: Rotation is in degrees in the anti-clockwise direction.
        inline float GetRotation() const { return m_Rotation; }
        // NOTE: Rotation is in degrees in the anti-clockwise direction.
        inline void SetRotation(float rotation) { m_Rotation = rotation; RecalculateMatrices(); }

        inline const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
        inline const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
        inline const glm::mat4& GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }
    private:
        void RecalculateMatrices();
    private:
        glm::mat4 m_ProjectionMatrix;
        glm::mat4 m_ViewMatrix;
        glm::mat4 m_ViewProjectionMatrix;

        glm::vec3 m_Position = glm::vec3(0.0f);
        float m_Rotation = 0.0f;  // In degrees, anti-clockwise
    };
}
