#ifndef RHEA_IMPORTER_H
#define RHEA_IMPORTER_H

#include <string>
#include "Mesh.h"

namespace Rhea
{
	class Application;

	class Importer
	{
	public:		
		bool load(Application* app, std::string objbuf, std::string mtlbuf, std::vector<Mesh::vertex>& vertices, std::vector<unsigned long>& indices);
	};
}

#endif // !RHEA_IMPORTER_H
