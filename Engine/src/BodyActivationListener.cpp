#include "Physics/BodyActivationListener.h"
#include <Jolt/Jolt.h>  // Include this to recognize JPH types
#include <Jolt/Physics/Body/Body.h>  // Specific header for JPH::BodyID
#include "Debug.h"

void Imp::Phys::BodyActivationListener::OnBodyActivated(const JPH::BodyID& inBodyID, JPH::uint64 inBodyUserData)
{
	Debug::Info("Body: {}, was activated", inBodyID.GetIndexAndSequenceNumber());
}

void Imp::Phys::BodyActivationListener::OnBodyDeactivated(const JPH::BodyID& inBodyID, JPH::uint64 inBodyUserData)
{
	Debug::Info("Body: {}, was deactivated", inBodyID.GetIndexAndSequenceNumber());
}
