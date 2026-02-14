#ifndef RHEA_GLTF_SCENE_H
#define RHEA_GLTF_SCENE_H

#include <Rhea/Unit.h>
#include <Rhea/Core/Geometry.h>

#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_INCLUDE_JSON
#define TINYGLTF_NO_INCLUDE_STB_IMAGE
#define TINYGLTF_NO_STB_IMAGE
#define TINIGLTF_NO_FS
#include "stb_image/stb_image.h"
#include <nlohmann/json.hpp>
#include <ThirdParty/tiny_gltf_loader/tiny_gltf.h>
#include <Components/Math/Vector2.h>
#include <Components/Math/Vector3.h>
#include "Modules/Animation/include/Rhea/Animation/Skeleton.h"

namespace Rhea
{
	class Material;

	class GltfScene : public Geometry
	{
		MECLASS(GltfScene, Geometry)
	public:
		GltfScene(std::string name, std::string type_name) :
			m_instance(nullptr),
			m_has_skeleton(false),
			Geometry(name, type_name)
		{}

		virtual ~GltfScene()
		{}

		bool load(Rhea::Mesh* mesh);

		void CreateSkeleton(int idx, Skeleton* skeleton);

		void CreateAnimations(Rhea::Mesh* mesh);

		void ready() override;

		// for loading the base
		std::vector<Unit::Export> getReferences();

		void SetModel(tinygltf::Model model) { m_model = model; }

		// Instantiate the scenes children to the scene given
		void Instantiate(Unit* owner);

		// set image indices
		void setImage(int idx, ImageResource* img) { m_images[idx] = img; }

		// get image by index
		ImageResource* getImage(int idx) { auto it = m_images.find(idx); if (it != m_images.end()) { return it->second; } }

	protected:
		// path to resource we will instance (if any)
		std::string m_instance_path;
		// if this is an instance this stores the link to the original
		GltfScene* m_instance;

		tinygltf::Model m_model;

		std::vector<unsigned long> ExtractIndices(const tinygltf::Accessor& accessor);
		void HandleNode(tinygltf::Node& node, Mesh* mesh);

		struct Vertex {
			Math::Vector3 position;
			Math::Vector3 normal;
			Math::Vector3 color;
			Math::Vector2 uv;
			Math::Vector3 tangent;

			bool operator==(const Vertex& other) const {
				return position.equals(other.position)/* && normal.equals(other.normal)*/ && uv.equals(other.uv)/* && color.equals(other.color) */;
			}
		};

		// Images
		std::map<int, ImageResource*> m_images;

		// Materials
		std::vector<Material*> m_materials;

		bool m_has_skeleton;

		// Map GLTF node indices to bone indices
		std::unordered_map<int, int> m_nodeIndexToBoneIndex;
	};
}

#endif // !RHEA_GLTF_SCENE_H
