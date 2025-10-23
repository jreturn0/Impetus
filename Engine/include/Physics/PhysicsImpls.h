#pragma once

#include "PhysicsCommon.h"

namespace imp::phys {

    static void TraceImpl(const char* inFMT, ...)
    {
        va_list list;
        va_start(list, inFMT);
        char buffer[1024];
        vsnprintf(buffer, sizeof(buffer), inFMT, list);
        va_end(list);
        Debug::Info("Physics Trace: {}", buffer);
    }

#ifdef JPH_ENABLE_ASSERTS
    // Callback for asserts, connect this to your own assert handler if you have one
    static bool AssertFailedImpl(const char* inExpression, const char* inMessage, const char* inFile, JPH::uint inLine)
    {
        try {
            Debug::FatalError("{}:{}: ({}) {}", inFile, static_cast<unsigned>(inLine), inExpression, inMessage != nullptr ? inMessage : "");
        }
        catch (std::exception& e) {
            Debug::FatalError("Exception: {}", e.what());
        }
        catch (...) {
            Debug::FatalError("Exception in AssertFailedImpl");
        }
        return true;
    };
#endif // JPH_ENABLE_ASSERTS
    // Filter that determines if a pair of object layers can collide
    class ObjectLayerPairFilterImpl : public JPH::ObjectLayerPairFilter
    {
    public:
        ObjectLayerPairFilterImpl() = default;

        virtual bool ShouldCollide(JPH::ObjectLayer inObject1, JPH::ObjectLayer inObject2) const override
        {
            const bool isMoving = (inObject1 & ObjectLayers::MOVING) || (inObject2 & ObjectLayers::MOVING);
            if (!isMoving)
                return false;
            const bool matchingLayers = (inObject1 & ~ObjectLayers::MOVING) == (inObject2 & ~ObjectLayers::MOVING);
            return matchingLayers;
        }
    };
    // BroadPhaseLayerInterface implementation
    // This defines a mapping between object and broadphase layers.
    class BPLayerInterfaceImpl final : public JPH::BroadPhaseLayerInterface
    {
    public:
        BPLayerInterfaceImpl() = default;
        virtual JPH::uint GetNumBroadPhaseLayers() const override { return imp::phys::BroadPhaseLayers::NUM_LAYERS; }
        virtual JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const override { JPH_ASSERT(inLayer < 16); return imp::phys::CastBroadPhaseLayer(inLayer & imp::phys::ObjectLayers::MOVING); }

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
        virtual const char* GetBroadPhaseLayerName(JPH::BroadPhaseLayer inLayer) const override
        {
            switch (static_cast<JPH::BroadPhaseLayer::Type>(inLayer)) {
            case static_cast<JPH::BroadPhaseLayer::Type>(imp::phys::BroadPhaseLayers::NON_MOVING):	return "NON_MOVING";
            case static_cast<JPH::BroadPhaseLayer::Type>(imp::phys::BroadPhaseLayers::MOVING):		return "MOVING";
            default:													JPH_ASSERT(false); return "INVALID";
            }
        }
#endif // JPH_EXTERNAL_PROFILE || JPH_PROFILE_ENABLED


    };

    // Class that determines if an object layer can collide with a broadphase layer
    class ObjectVsBroadPhaseLayerFilterImpl final : public JPH::ObjectVsBroadPhaseLayerFilter
    {
    public:
        virtual bool ShouldCollide(JPH::ObjectLayer inLayer1, JPH::BroadPhaseLayer inLayer2) const override
        {
            if (!(inLayer1 & imp::phys::ObjectLayers::MOVING))
                return inLayer2 == JPH::BroadPhaseLayer(imp::phys::BroadPhaseLayers::MOVING);
            if (inLayer1 & imp::phys::ObjectLayers::MOVING)
                return true;
            return false;
        }
    };
}