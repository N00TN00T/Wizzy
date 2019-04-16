#pragma once

#include "Wizzy/Input.h"

namespace Wizzy {

    class MacosxInput
        : public Input {        
    protected:
        virtual bool GetKeyImpl(KeyCode keycode) override;
        virtual bool GetMouseButtonImpl(MouseCode mousecode) override;
		virtual vec2 GetMousePosImpl() override;
    };

    

}