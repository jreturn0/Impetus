#pragma once

#include "PhysicsCommon.h"

namespace Imp::Phys {
	struct RayHit
	{
		bool hit;
		entt::entity entity = entt::null;
		JPH::BodyID body = JPH::BodyID{ JPH::BodyID::cInvalidBodyID };
		glm::vec3 point{};
		glm::vec3 normal{};
		float fraction=-1;
		operator bool() const
		{
			return hit;
		}

	};
	static void TraceImpl(const char* inFMT, ...)
	{
		// Format the message
		va_list list;
		va_start(list, inFMT);
		char buffer[1024];
		vsnprintf(buffer, sizeof(buffer), inFMT, list);
		va_end(list);

		// Print to the TTY
		Debug::Info("Trace: {}", buffer);
	}
#ifdef JPH_ENABLE_ASSERTS
	// Callback for asserts, connect this to your own assert handler if you have one
	static bool AssertFailedImpl(const char* inExpression, const char* inMessage, const char* inFile, JPH::uint inLine)
	{
		try {
			// Print to the TTY
			//Debug::Out("{}:{}: ({}) {}", inFile, static_cast<unsigned>(inLine), inExpression, inMessage != nullptr ? inMessage : "");


			Debug::FatalError("{}:{}: ({}) {}", inFile, static_cast<unsigned>(inLine), inExpression, inMessage != nullptr ? inMessage : "");
		} catch (std::exception& e) {
			Debug::FatalError("Exception: {}", e.what());
		} catch (...) {
			Debug::FatalError("Exception in AssertFailedImpl");
		}
		// Breakpoint
		return true;
	};
#endif // JPH_ENABLE_ASSERTS
	class ObjectLayerPairFilterImpl : public JPH::ObjectLayerPairFilter
	{
	private:

	public:
		ObjectLayerPairFilterImpl() = default;

		virtual bool ShouldCollide(JPH::ObjectLayer inObject1, JPH::ObjectLayer inObject2) const override
		{
			//Debug::Info("ObjectLayerPairFilterImpl::ShouldCollide({}, {})", (uint16_t)(inObject1 & ObjectLayers::MOVING),(uint16_t)(inObject2 & ObjectLayers::MOVING));

			const bool isMoving = (inObject1 & ObjectLayers::MOVING) || (inObject2 & ObjectLayers::MOVING);
			if (!isMoving)
				return false;

			const bool matchingLayers = (inObject1 & ~ObjectLayers::MOVING) == (inObject2 & ~ObjectLayers::MOVING);
			return matchingLayers;


			switch (inObject1& ObjectLayers::MOVING) {
			case ObjectLayers::NON_MOVING:
				return (inObject2 & ObjectLayers::MOVING)== ObjectLayers::NON_MOVING;
			case ObjectLayers::MOVING:
				return (inObject2 & ObjectLayers::MOVING)== ObjectLayers::MOVING;
			default:
				break;

			}

			return ((inObject1 & ObjectLayers::MOVING) | (inObject2 & ObjectLayers::MOVING)) ? (inObject1 & ~ObjectLayers::MOVING) & (inObject2 & ~ObjectLayers::MOVING) : false;
		}
	};
	// BroadPhaseLayerInterface implementation
// This defines a mapping between object and broadphase layers.
	class BPLayerInterfaceImpl final : public JPH::BroadPhaseLayerInterface
	{
	public:



		BPLayerInterfaceImpl() = default;


		virtual JPH::uint					GetNumBroadPhaseLayers() const override
		{
			return Imp::Phys::BroadPhaseLayers::NUM_LAYERS;
		}

		virtual JPH::BroadPhaseLayer			GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const override
		{
			JPH_ASSERT(inLayer < 16);
			return Imp::Phys::CastBroadPhaseLayer(inLayer & Imp::Phys::ObjectLayers::MOVING);
		}

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
		virtual const char* GetBroadPhaseLayerName(JPH::BroadPhaseLayer inLayer) const override
		{
			switch (static_cast<JPH::BroadPhaseLayer::Type>(inLayer)) {
			case static_cast<JPH::BroadPhaseLayer::Type>(Imp::Phys::BroadPhaseLayers::NON_MOVING):	return "NON_MOVING";
			case static_cast<JPH::BroadPhaseLayer::Type>(Imp::Phys::BroadPhaseLayers::MOVING):		return "MOVING";
			default:													JPH_ASSERT(false); return "INVALID";
			}
		}
#endif // JPH_EXTERNAL_PROFILE || JPH_PROFILE_ENABLED


	};

	/// Class that determines if an object layer can collide with a broadphase layer
	class ObjectVsBroadPhaseLayerFilterImpl final : public JPH::ObjectVsBroadPhaseLayerFilter
	{
	public:
		virtual bool ShouldCollide(JPH::ObjectLayer inLayer1, JPH::BroadPhaseLayer inLayer2) const override
		{
			//Debug::Info("ObjectVsBroadPhaseLayerFilterImpl::ShouldCollide({}, {})", static_cast<uint16_t>(inLayer1 & Imp::Phys::ObjectLayers::MOVING), (uint16_t)inLayer2.GetValue());
			if (!(inLayer1 & Imp::Phys::ObjectLayers::MOVING))
				return inLayer2 == JPH::BroadPhaseLayer(Imp::Phys::BroadPhaseLayers::MOVING);
			if (inLayer1 & Imp::Phys::ObjectLayers::MOVING)
				return true;
			return false;
		}
	};
}