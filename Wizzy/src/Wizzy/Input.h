#pragma once

#include "Wizzy/Core.h"
#include "Wizzy/KeyCodes.h"
#include "Wizzy/MouseButtonCodes.h"

namespace Wizzy {

    typedef u16 KeyCode;
    typedef u8 MouseCode;

    class  Input {
    public:
        inline static bool GetKey(KeyCode keycode) { 
            return s_instance->GetKeyImpl(keycode); 
        }
        inline static bool GetMouseButton(MouseCode mousecode) {
            return s_instance->GetMouseButtonImpl(mousecode);
        }
		inline static vec2 GetMousePos() {
			return s_instance->GetMousePosImpl();
		}

    protected:
        virtual bool GetKeyImpl(KeyCode keycode) = 0;
        virtual bool GetMouseButtonImpl(MouseCode mousecode) = 0;
		virtual vec2 GetMousePosImpl() = 0;

        inline void SetInstance(Input *instance) {
            s_instance = instance;
        }

    private:
        static Input *s_instance;
    };

}