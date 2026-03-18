add_library(RheaClient STATIC)
add_library(Rhea::Client ALIAS RheaClient)

target_sources(RheaClient PRIVATE
	"Graphics/GraphicsService.cpp" "Graphics/Texture.cpp" "Graphics/ItemManager.cpp" "Graphics/SceneManager.cpp" 
	"Graphics/CameraManager.cpp" "Graphics/GraphicsManager.h" "Graphics/MaterialManager.cpp"
	"Graphics/GraphicsManager.cpp"  "Graphics/MeshManager.h" "Graphics/MeshManager.cpp" 
 "Fabrics/PoseidonGraphics/include/Rhea/PoseidonGraphicsFabric.h" "Fabrics/PoseidonGraphics/src/PoseidonGraphicsFabric.cpp" 
 "Graphics/MeshGpuCache.h" "Graphics/MeshGpuCache.cpp" "Graphics/TextureGpuCache.h" "Graphics/TextureGpuCache.cpp" 
 "Graphics/MaterialGpuCache.h" "Graphics/MaterialGpuCache.cpp")

target_include_directories(RheaClient PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}       # so includes like <Graphics/...> still work temporarily
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/Fabrics/PoseidonGraphics/include
)

target_link_libraries(RheaClient PUBLIC
  Rhea::Core
  Rhea::GUI
  Meworla::Poseidon
  Winmm
)

# helper
add_library(RheaClientRuntime INTERFACE)
add_library(Rhea::ClientRuntime ALIAS RheaClientRuntime)

target_link_libraries(RheaClientRuntime INTERFACE
  Rhea::Runtime
  Rhea::Client
)

# Register this fabric for GeneratedFabrics.cpp generation
rhea_register_fabric("${CMAKE_CURRENT_SOURCE_DIR}/Fabrics/PoseidonGraphics/fabric.json")