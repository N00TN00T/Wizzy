#pragma once

#include "Wizzy/Core.h"
#include "Wizzy/KeyCodes.h"
#include "Wizzy/MouseButtonCodes.h"

namespace Wizzy {

    typedef u16 KeyCode;
    typedef u8 MouseCode;

    class WZ_API Input {
    public:
        inline static bool GetKey(KeyCode keycode) { 
            return s_instance->GetKeyImpl(keycode); 
        }
        inline static bool GetMouseButton(MouseCode mousecode) {
            return s_instance->GetMouseButtonImpl(mousecode);
        }
        inline static float GetMouseX() {
            return s_instance->GetMouseXImpl();
        }
        inline static float GetMouseY() {
            return s_instance->GetMouseYImpl();
        }

    protected:
        virtual bool GetKeyImpl(KeyCode keycode) = 0;
        virtual bool GetMouseButtonImpl(MouseCode mousecode) = 0;
        virtual float GetMouseXImpl() = 0;
        virtual float GetMouseYImpl() = 0;

        inline void SetInstance(Input *instance) {
            s_instance = instance;
        }

    private:
        static Input *s_instance;
    };

}