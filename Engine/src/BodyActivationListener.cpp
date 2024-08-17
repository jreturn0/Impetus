#include "Physics/BodyActivationListener.h"

void Imp::Phys::BodyActivationListener::OnBodyActivated(const JPH::BodyID& inBodyID, JPH::uint64 inBodyUserData)
{
	Debug::Info("A body got activated");
}

void Imp::Phys::BodyActivationListener::OnBodyDeactivated(const JPH::BodyID& inBodyID, JPH::uint64 inBodyUserData)
{
	Debug::Info("A body got deactivated");
}
