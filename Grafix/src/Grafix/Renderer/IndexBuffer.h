#pragma once

namespace Grafix
{
    class IndexBuffer
    {
    public:
        virtual ~IndexBuffer() = default;

        virtual void Bind() = 0;
        virtual void Unbind() = 0;

        virtual inline uint32_t GetCount() const = 0;

        static Shared<IndexBuffer> Create(uint32_t* indices, uint32_t count);
    };
}

