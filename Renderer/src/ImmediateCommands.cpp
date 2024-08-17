#include "core/ImmediateCommands.h"

#include "Debug.h"
#include "core/CommandBuffer.h"
#include "core/CommandPool.h"

#include "core/Device.h"
Imp::Render::ImmediateCommands::ImmediateCommands(const Device& device, uint32_t familyIndex)
{
	commandPool = CreateUniqueCommandPool(device, familyIndex);
	commandBuffer = CreateUniqueCommandBuffer(device, *commandPool);
	fence = device.getLogical().createFenceUnique(vk::FenceCreateInfo(vk::FenceCreateFlagBits::eSignaled));
}

Imp::Render::ImmediateCommands::~ImmediateCommands()
{

    
    //std::cout << "ImmediateCommands destructor" << std::endl;
}

void Imp::Render::ImmediateCommands::immediateSubmit(const vk::Queue& queue, const vk::Device& device, std::function<void(vk::CommandBuffer& cmd)>&& function) const
{
    // Ensure command buffer and fence are initialized
    if (!commandBuffer || !fence) {
        Debug::Throw( "Command buffer or fence not initialized.");
    }

    // Reset fence
   (device.resetFences(fence.get()));

    // Reset command buffer
    vk::CommandBuffer commandBufferHandle = commandBuffer->getBuffer();
   (commandBufferHandle.reset());

    vk::CommandBufferBeginInfo beginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
   (commandBufferHandle.begin(beginInfo));

    function(commandBufferHandle);

    (commandBufferHandle.end());

    vk::CommandBufferSubmitInfo submitInfo(commandBufferHandle);
    vk::SubmitInfo2 submitInfo2({}, {}, submitInfo);
    (queue.submit2(submitInfo2, *fence));

    vkutil::CheckResult(device.waitForFences(fence.get(), VK_TRUE, UINT64_MAX));

}
