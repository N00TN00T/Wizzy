#include "wzpch.h"

#include "Wizzy/Extensions/Game/CharacterControllers.h"

WZ_NS_BEGIN(Wizzy);
WZ_NS_BEGIN(Game);

CharacterController2D::CharacterController2D() {
    Subscribe(EventType::app_update);
}

bool CharacterController2D::ProcessComponents(
                        const Event& e, 
                        CharacterController2DInputMapping* controllerInputMapping,
                        CharacterController2DConfig* controllerConfig,
                        Position2D* position
                        ) const {
    switch (e.GetEventType()) {
        case EventType::app_update:
        {
            // TODO: Optional rigidbody that move object in physics world
            // instead of directly in the game world

            const AppUpdateEvent& updateEvent = (const AppUpdateEvent&)e;

            fvec2 move;
            if (Input::GetKey(controllerInputMapping->upKey)) {
                move.y += controllerConfig->baseSpeed;
            }
            if (Input::GetKey(controllerInputMapping->rightKey)) {
                move.x += controllerConfig->baseSpeed;
            }
            if (Input::GetKey(controllerInputMapping->downKey)) {
                move.y -= controllerConfig->baseSpeed;
            }
            if (Input::GetKey(controllerInputMapping->leftKey)) {
                move.x -= controllerConfig->baseSpeed;
            }

            // Diagonal movement
            if (move.x != 0 && move.y != 0) {
                move *= .7f;
            }

            position->value += move * updateEvent.GetDeltaTime();

            return false;
        }
    }
}

WZ_NS_END;
WZ_NS_END;