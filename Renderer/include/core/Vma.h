#pragma once


#include "utils/QuickMacros.h"
#include <vma/vk_mem_alloc.h>
namespace Imp::Render
{

	class Vma
	{
	private:
		VmaAllocator vma;

	public:
		DISABLE_COPY_AND_MOVE(Vma);
		Vma(const class Instance& instance,const class Device& device);
		VmaAllocator& getAllocator() { return vma; }
		operator VmaAllocator& () { return vma; }
		~Vma();
	};
}
