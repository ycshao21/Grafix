#pragma once

namespace Grafix
{
    enum class ShaderDataType : uint8_t
    {
        None = 0,
        Float, Float2, Float3, Float4,
        Mat3, Mat4,
        Int, Int2, Int3, Int4,
        Bool
    };

    static uint32_t ShaderDataTypeSize(ShaderDataType type)
    {
        switch (type)
        {
            case ShaderDataType::Float:     return 4;
            case ShaderDataType::Float2:    return 4 * 2;
            case ShaderDataType::Float3:    return 4 * 3;
            case ShaderDataType::Float4:    return 4 * 4;
            case ShaderDataType::Mat3:      return 4 * 3 * 3;
            case ShaderDataType::Mat4:      return 4 * 4 * 4;
            case ShaderDataType::Int:       return 4;
            case ShaderDataType::Int2:      return 4 * 2;
            case ShaderDataType::Int3:      return 4 * 3;
            case ShaderDataType::Int4:      return 4 * 4;
            case ShaderDataType::Bool:      return 1;
        }

        GF_CORE_ASSERT(false, "Unknown shader data type!");
        return 0;
    }

    struct VertexBufferElement
    {
        std::string Name;
        ShaderDataType Type;
        uint32_t Offset;
        uint32_t Size;
        bool Normalized;

        VertexBufferElement() = default;

        VertexBufferElement(ShaderDataType type, const std::string& name, bool normalized = false)
            : Name(name), Type(type), Offset(0), Size(ShaderDataTypeSize(type)), Normalized(normalized)
        {
        }

        uint32_t GetComponentCount() const
        {
            switch (Type)
            {
                case ShaderDataType::Float:     return 1;
                case ShaderDataType::Float2:    return 2;
                case ShaderDataType::Float3:    return 3;
                case ShaderDataType::Float4:    return 4;
                case ShaderDataType::Mat3:      return 3 * 3;
                case ShaderDataType::Mat4:      return 4 * 4;
                case ShaderDataType::Int:       return 1;
                case ShaderDataType::Int2:      return 2;
                case ShaderDataType::Int3:      return 3;
                case ShaderDataType::Int4:      return 4;
                case ShaderDataType::Bool:      return 1;
            }

            GF_CORE_ASSERT(false, "Unknown shader data type!");
            return 0;
        }
    };

    class VertexBufferLayout
    {
    public:
        VertexBufferLayout() = default;
        VertexBufferLayout(const std::initializer_list<VertexBufferElement>& elements);

        inline const std::vector<VertexBufferElement>& GetElements() const { return m_Elements; }
        inline uint32_t GetStride() const { return m_Stride; }

        inline size_t Size() const { return m_Elements.size(); }

        std::vector<VertexBufferElement>::const_iterator begin() const { return m_Elements.begin(); }
        std::vector<VertexBufferElement>::const_iterator end() const { return m_Elements.end(); }

        VertexBufferElement& operator[](size_t index) { return m_Elements[index]; }
        const VertexBufferElement& operator[](size_t index) const { return m_Elements[index]; }
    private:
        void CalcOffsetsAndStride();
    private:
        std::vector<VertexBufferElement> m_Elements;
        uint32_t m_Stride = 0;
    };

    class VertexBuffer
    {
    public:
        virtual ~VertexBuffer() = default;

        virtual void Bind() = 0;
        virtual void Unbind() = 0;

        virtual const VertexBufferLayout& GetLayout() const = 0;
        virtual void SetLayout(const VertexBufferLayout& layout) = 0;

        static Shared<VertexBuffer> Create(float* vertices, uint32_t bytes);
    };
}
