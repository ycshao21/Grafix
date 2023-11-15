#pragma once

#include "Event.h"
#include "Grafix/Core/Input/Codes.h"

namespace Grafix
{
    class KeyEvent : public Event
    {
    public:
        inline Key GetKey() const { return m_Key; }

        EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)
    protected:
        KeyEvent(Key keycode) : m_Key(keycode) {}

        Key m_Key;
    };

    class KeyPressedEvent final : public KeyEvent
    {
    public:
        KeyPressedEvent(Key key, bool isRepeat) : KeyEvent(key), m_IsRepeat(isRepeat) {}

        inline bool IsRepeat() const { return m_IsRepeat; }

        std::string ToString() const override
        {
            return std::format("KeyPressedEvent: {0} (repeat = {1})", (uint16_t)m_Key, (uint16_t)m_IsRepeat);
        }

        EVENT_CLASS_TYPE(KeyPressed)
    private:
        bool m_IsRepeat;
    };

    class KeyReleasedEvent final : public KeyEvent
    {
    public:
        KeyReleasedEvent(Key key) : KeyEvent(key) {}

        std::string ToString() const override
        {
            return std::format("KeyReleasedEvent: {0}", (uint16_t)m_Key);
        }

        EVENT_CLASS_TYPE(KeyReleased)
    };

    class KeyTypedEvent final : public KeyEvent
    {
    public:
        KeyTypedEvent(Key key) : KeyEvent(key) {}

        std::string ToString() const override
        {
            return std::format("KeyTypedEvent: {0}", (uint16_t)m_Key);
        }

        EVENT_CLASS_TYPE(KeyTyped)
    };
}
