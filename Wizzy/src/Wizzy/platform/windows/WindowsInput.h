#pragma once

#include "Wizzy/Input/Input.h"

namespace Wizzy {

    class WindowsInput
        : public Input {        
    protected:
        virtual bool GetKeyImpl(KeyCode keycode) override;
        virtual bool GetMouseButtonImpl(MouseCode mousecode) override;
		virtual fvec2 GetMousePosImpl() override;
    };

    

}