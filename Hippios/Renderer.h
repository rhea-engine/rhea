#ifndef HIPPIOS_RENDERER_H
#define HIPPIOS_RENDERER_H

#include <string>

#include "BaseTypes.h"
#include "TextureDesc.h"
#include "PipelineState.h"
#include "BlendState.h"
#include "Sampler.h"
#include "SwapChain.h"
#include "Texture.h"
#include "GraphicsBuffer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "GPUProgram.h"
#include "RenderTarget.h"

namespace Hippios
{
	/**
	* @class Renderer
	* @brief Abstract base class defining a rendering API interface.
	*
	* The Renderer class provides a platform-independent interface for graphics rendering.
	* It includes methods for creating buffers, textures, shaders, and rendering states,
	* as well as functions for binding resources and issuing draw calls.
	*/
	class Renderer
	{
	public:
		/// Virtual destructor to ensure proper cleanup in derived classes.
		virtual ~Renderer();

		/**
		 * @brief Creates a vertex buffer.
		 * @param vertexSize The size (in bytes) of a single vertex.
		 * @param numVertices The total number of vertices in the buffer.
		 * @param dynamic If true, the buffer is dynamic and can be updated frequently.
		 * @return A pointer to the created VertexBuffer.
		 */
		virtual VertexBuffer* createVertexBuffer(size_t vertexSize, size_t numVertices, bool dynamic) = 0;

		/**
		 * @brief Creates an index buffer.
		 * @param vertex_size The size of each index entry.
		 * @param num_faces The total number of faces (triangles/quads) in the buffer.
		 * @param dynamic If true, the buffer is dynamic and can be updated frequently.
		 * @return A pointer to the created IndexBuffer.
		 */
		virtual IndexBuffer* createIndexBuffer(size_t vertex_size, size_t num_faces, bool dynamic) = 0;

		/**
		 * @brief Creates a generic graphics buffer.
		 * @param size_in_bytes The total size of the buffer in bytes.
		 * @return A pointer to the created GraphicsBuffer.
		 */
		virtual GraphicsBuffer* createGraphicsBuffer(size_t size_in_bytes) = 0;

		/**
		 * @brief Creates a texture.
		 * @param desc The descriptor structure defining the texture properties.
		 * @return A pointer to the created Texture.
		 */
		virtual Texture* createTexture(TextureDesc& desc) = 0;

		/**
		 * @brief Clears the given render target.
		 * @param render_target The render target to be cleared.
		 */
		virtual void clearPass(RenderTarget* render_target) = 0;

		/**
		 * @brief Presents the current swap chain to the screen.
		 * @param swap_chain The swap chain to be presented.
		 * @return An integer status code (0 if successful, non-zero on failure).
		 */
		virtual int presentPass(SwapChain* swap_chain) = 0;

		/**
		 * @brief Loads and compiles a shader program from a file.
		 * @param filename The path to the shader file.
		 * @param shader_type The type of shader (e.g., "VS" for vertex shader, "PS" for pixel shader).
		 * @param entry_point The entry function name within the shader file.
		 * @return A pointer to the created GPUProgram.
		 */
		virtual GPUProgram* addMaterial(std::string filename, char* shader_type, char* entry_point) = 0;

		/**
		 * @brief Loads and compiles a shader program from memory.
		 * @param buf The shader source code in memory.
		 * @param buflen The length of the buffer in bytes.
		 * @param shader_type The type of shader (e.g., "VS" for vertex shader, "PS" for pixel shader).
		 * @param entry_point The entry function name within the shader source.
		 * @return A pointer to the created GPUProgram.
		 */
		virtual GPUProgram* addMaterial(unsigned char* buf, int buflen, char* shader_type, char* entry_point) = 0;

		/**
		 * @brief Creates a swap chain for rendering to a window.
		 * @param win The native window handle.
		 * @return A pointer to the created SwapChain.
		 */
		virtual SwapChain* createSwapChain(NativeWindow* win) = 0;

		/**
		 * @brief Creates a render target associated with a swap chain.
		 * @param width The width of the render target in pixels.
		 * @param height The height of the render target in pixels.
		 * @param swap_chain The associated swap chain.
		 * @return A pointer to the created RenderTarget.
		 */
		virtual RenderTarget* createRenderTarget(int width, int height, SwapChain* swap_chain) = 0;

		/**
		 * @brief Creates a pipeline state object.
		 * @param desc A descriptor structure defining the pipeline state properties.
		 * @return A pointer to the created PipelineStateObject.
		 */
		virtual PipelineStateObject* createPipelineState(PipelineStateObjectDesc* desc) = 0;

		/**
		 * @brief Sets the active pipeline state.
		 * @param pso The pipeline state object to set.
		 */
		virtual void setPipelineState(PipelineStateObject* pso) = 0;

		/**
		 * @brief Binds a vertex and index buffer for rendering.
		 * @param vb The vertex buffer to bind.
		 * @param ib The index buffer to bind.
		 */
		virtual void bindBuffers(VertexBuffer* vb, IndexBuffer* ib) = 0;

		/**
		 * @brief Sets the active vertex shader.
		 * @param material The GPU program containing the vertex shader.
		 */
		virtual void setVertexShader(GPUProgram* material) = 0;

		/**
		 * @brief Sets the active pixel shader.
		 * @param material The GPU program containing the pixel shader.
		 */
		virtual void setPixelShader(GPUProgram* material) = 0;

		/**
		 * @brief Binds a texture to a shader slot.
		 * @param texture The texture to bind.
		 * @param slot The texture slot (default is 0).
		 */
		virtual void setTexture(Texture* texture, int slot = 0) = 0;

		/**
		 * @brief Draws non-indexed geometry.
		 * @param size The number of vertices to draw.
		 * @param start_vtx The starting vertex index.
		 */
		virtual void draw(int size, int start_vtx) = 0;

		/**
		 * @brief Draws indexed geometry using the bound vertex and index buffers.
		 * @param size The number of indices to draw.
		 * @param start_idx The starting index.
		 * @param start_vtx The starting vertex index.
		 */
		virtual void drawIndexed(int size, int start_idx, int start_vtx) = 0;

		/**
		 * @brief Sets the blending state.
		 * @param state The blend state to apply.
		 * @param blend_factor A four-component blend factor.
		 * @param mask The blend mask.
		 */
		virtual void setBlendState(BlendState* state, float blend_factor[4], unsigned int mask) = 0;

		/**
		 * @brief Sets the stencil state.
		 * @param state The stencil state to apply.
		 * @param stencil_ref The stencil reference value.
		 */
		virtual void setStencilState(StencilState* state, int stencil_ref) = 0;

		/**
		 * @brief Retrieves a previously created stencil state.
		 * @param name The name of the stencil state.
		 * @return A pointer to the requested StencilState.
		 */
		virtual StencilState* getStencilState(std::string name) = 0;

		/**
		 * @brief Creates a new stencil state.
		 * @param name The name for the stencil state.
		 * @param desc The descriptor defining the stencil state properties.
		 * @return A pointer to the created StencilState.
		 */
		virtual StencilState* createStencilState(std::string name, DepthStencilStateDesc* desc) = 0;

		/**
		 * @brief Creates a new sampler state.
		 * @param name The name for the sampler.
		 * @param desc The descriptor defining the sampler properties.
		 * @return True if the sampler was successfully created, false otherwise.
		 */
		virtual bool createSampler(std::string name, SamplerDesc* desc) = 0;

		/**
		 * @brief Retrieves a sampler handle by name.
		 * @param name The name of the sampler.
		 * @return A pointer to the GPUHandle representing the sampler.
		 */
		virtual GPUHandle* getSampler(std::string name) = 0;

		/**
		 * @brief Sets the active sampler.
		 * @param handle The handle to the sampler.
		 */
		virtual void setSampler(GPUHandle* handle) = 0;

		/**
		 * @brief Binds a graphics buffer to a specific shader stage.
		 * @param buffer The buffer to bind.
		 * @param slot The binding slot (default is 0).
		 * @param stage The shader stage to bind to (e.g., "VS" for vertex shader).
		 */
		virtual void bindGPUBuffer(GraphicsBuffer* buffer, int slot = 0, char* stage = "VS") = 0;

		/**
		 * @brief Initializes a GPU buffer for shader input.
		 * @param size The size of the buffer in bytes.
		 * @return A pointer to the initialized GraphicsBuffer.
		 */
		virtual GraphicsBuffer* initGPUBuffer(size_t size) = 0;
	};
}
#endif // !HIPPIOS_RENDERER_H
