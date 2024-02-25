#ifndef RHEA_VERTEX_ATTRIBUTE_H
#define RHEA_VERTEX_ATTRIBUTE_H

#include <string>

namespace Rhea {
	// a type of the vertex attribute
	enum class VertexAttributeType {
		UNKNOWN, 
		POSITION,
		NORMAL,
		TEXCOORD,
		COLOR,
		TANGENT,
		BITANGENT,
		INDEX
	};

	struct VertexAttribute {
		std::string name; // attribute name
		VertexAttributeType type; // attribute type
		int element_format; // format (element size in bytes)
		int element_count; // count
		int size; // size in bytes (=format*count)
		int offset; // offset in bytes from the start of the vertex
	};
}
#endif // !RHEA_VERTEX_ATTRIBUTE_H
