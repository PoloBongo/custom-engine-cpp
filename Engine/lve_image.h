#pragma once

#include "lve_device.h"
#include "lve_buffer.h"

namespace lve {
	class LveImage
	{
	public:

		struct Builder {
		};

		LveImage(LveDevice& _lveDevice, const LveImage::Builder& _builder);
		~LveImage();

		LveImage(const LveImage&) = delete;
		LveImage operator=(const LveImage&) = delete;

	private:
		void createTextureImage();
	};
}
