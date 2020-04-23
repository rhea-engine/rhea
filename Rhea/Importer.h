#ifndef RHEA_IMPORTER_H
#define RHEA_IMPORTER_H

#include <string>
#include "Poseidon/Mesh.h"

namespace Rhea
{
	class Application;

	class Importer
	{
	public:		
		bool load(Application* app, std::string objbuf, std::string mtlbuf, std::vector<Hippios::vertex_uv>& vertices, std::vector<unsigned long>& indices);
	};
}

#endif // !RHEA_IMPORTER_H
