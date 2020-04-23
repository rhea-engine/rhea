#ifndef RHEA_IMAGE_RESOURCE
#define RHEA_IMAGE_RESOURCE

#include <Poseidon/Image.h>
#include "Resource.h"

namespace Rhea
{
	class ImageResource : public Resource
	{
	public:
		ImageResource(std::string basename, std::string type_name) :
			m_img(NULL),
			Resource(basename, type_name)
		{}
		~ImageResource();
		void load(ReadyHandler handler);

		Poseidon::Image* get() { return m_img; }

		// factory function
		static Unit* createType(std::string name, std::string type);

	protected:
		Poseidon::Image* m_img;
	};
}


#endif // !RHEA_IMAGE_RESOURCE
