#ifndef AA32B710_B7A4_48B9_AC32_AF4954B60C38
#define AA32B710_B7A4_48B9_AC32_AF4954B60C38
#ifndef DC95ECE2_4E01_4EFA_8058_92A36917FC0C
#define DC95ECE2_4E01_4EFA_8058_92A36917FC0C

#include "PosColVertex.hpp"
#include "PosNorTexVertex.hpp"
#include "Light.hpp"
#include "mat4.hpp"


#include "RTG.hpp"

struct A3 : RTG::Application {

	A3(RTG &);
	A3(A3 const &) = delete; //you shouldn't be copying this object
	~A3();

	//kept for use in destructor:
	RTG &rtg;

	//--------------------------------------------------------------------
	//Resources that last the lifetime of the application:

	//chosen format for depth buffer:
	VkFormat depth_format{};
	//Render passes describe how pipelines write to images:
	VkRenderPass render_pass = VK_NULL_HANDLE;

	//Pipelines:

	struct BackgroundPipeline {
		//no desscriptor set layouts

		struct Push {
			float time;
		};

		VkPipelineLayout layout = VK_NULL_HANDLE;

		//no vertex bindings

		VkPipeline handle = VK_NULL_HANDLE;

		void create(RTG &, VkRenderPass renger_pass, uint32_t subpass);

		void destroy (RTG &);
	} background_pipeline;

	struct LinesPipeline {
		//descriptor set layouts:
		VkDescriptorSetLayout set0_Camera = VK_NULL_HANDLE;

		//types for descriptors:
		struct Camera {
			mat4 CLIP_FROM_WORLD;
		};
		static_assert(sizeof(Camera) == 16*4, "camera buffer structure is packed");

		//no push constants

		VkPipelineLayout layout = VK_NULL_HANDLE;

		using Vertex = PosColVertex;

		VkPipeline handle = VK_NULL_HANDLE;

		void create(RTG &, VkRenderPass render_pass, uint32_t subpass);
		void destroy(RTG &);
	} lines_pipeline;

	struct ObjectsPipeline {
		//descriptor set layouts:
		VkDescriptorSetLayout set0_World = VK_NULL_HANDLE;
		VkDescriptorSetLayout set1_Transforms = VK_NULL_HANDLE;
		VkDescriptorSetLayout set2_TEXTURE = VK_NULL_HANDLE;
		VkDescriptorSetLayout set3_ENVIRONMENT = VK_NULL_HANDLE;
		VkDescriptorSetLayout set4_Normal = VK_NULL_HANDLE;
		VkDescriptorSetLayout set5_Displacement = VK_NULL_HANDLE;


		
		//types for descriptors: TODO swap with light
		using World = Light;
		// struct World {
		// 	struct { float x, y, z, padding_; } SKY_DIRECTION;
		// 	struct { float r, g, b, padding_; } SKY_ENERGY;
		// 	struct { float x, y, z, padding_; } SUN_DIRECTION;
		// 	struct { float r, g, b, padding_; } SUN_ENERGY;
		// };
		// static_assert(sizeof(World) == 4*4 + 4*4 + 4*4 + 4*4, "World is the expected size.");

		struct Transform {
			mat4 CLIP_FROM_LOCAL;
			mat4 WORLD_FROM_LOCAL;
			mat4 WORLD_FROM_LOCAL_NORMAL;
		};
		static_assert(sizeof(Transform) == 16*4 + 16*4 + 16*4, 
						"Transform is the expected size.");

		//no push constants

		VkPipelineLayout layout = VK_NULL_HANDLE;

		using Vertex = PosNorTexVertex;

		int matType = 0;

		VkPipeline handle = VK_NULL_HANDLE;

		void create(RTG &, VkRenderPass render_pass, uint32_t subpass);
		void destroy(RTG &);
	} objects_pipeline;

	enum MatType {
		LAMBERTIAN = 0,
		PBR = 1,
		MIRROR = 2,
		ENVIRONMENT = 3,
	};


	//pools from which per-workspace things are allocated:
	VkCommandPool command_pool = VK_NULL_HANDLE;
	VkDescriptorPool descriptor_pool = VK_NULL_HANDLE;

	//workspaces hold per-render resources:
	struct Workspace {
		VkCommandBuffer command_buffer = VK_NULL_HANDLE; //from the command pool above; reset at the start of every render.

		//location for lines data: (streamed to GPU per-frame)
		Helpers::AllocatedBuffer lines_vertices_src; //host coherent; mapped
		Helpers::AllocatedBuffer lines_vertices; //device-local

		//location for LinePipeline::Camera data: (streamed to GPU per-frame)
		Helpers::AllocatedBuffer Camera_src; //host coherent; mapped
		Helpers::AllocatedBuffer Camera; //device-local
		VkDescriptorSet Camera_descriptors; //references Camera

		//location for ObjectsPipeline::World data: (streamed to GPU per-frame)
		Helpers::AllocatedBuffer World_src; //host coherent; mapped
		Helpers::AllocatedBuffer World; //device-local
		VkDescriptorSet World_descriptors; //references World

		//location for ObjectPipeline::Transforms data: (streamed to GPU per-frame)
		Helpers::AllocatedBuffer Transforms_src; //host coherent; mapped
		Helpers::AllocatedBuffer Transforms; //device-loval
		VkDescriptorSet Transforms_descriptors; //references Transforms

	};
	std::vector< Workspace > workspaces;

	//-------------------------------------------------------------------
	//static scene resources:

	struct AABB {
		float minX;
		float minY;
		float minZ;
		float maxX;
		float maxY;
		float maxZ;
	};

	struct Frustum {
		float near_right;
		float near_top;
		float near_plane;
		float far_plane;
	} frustum;

	void create_frustum();	

	Helpers::AllocatedBuffer object_vertices;
	struct ObjectVertices {
		uint32_t first = 0;
		uint32_t count = 0;
	};

	ObjectVertices plane_vertices;
	ObjectVertices torus_vertices;
	ObjectVertices sphere_vertices;
	std::map<std::string, ObjectVertices> mesh_vertices;
	std::map<std::string, AABB> mesh_box;
	

	std::vector< Helpers::AllocatedImage > textures;
	std::vector< VkImageView > texture_views;
	VkSampler texture_sampler = VK_NULL_HANDLE;
	VkDescriptorPool texture_descriptor_pool = VK_NULL_HANDLE;
	std::vector< VkDescriptorSet > texture_descriptors; //allocated from texture_descriptor_pool


	VkDescriptorPool environment_descriptor_pool = VK_NULL_HANDLE;
	VkDescriptorSet environment_descritor;

	std::vector< Helpers::AllocatedImage > normals;
	std::vector< VkImageView > normal_views;
	VkSampler normal_sampler = VK_NULL_HANDLE;
	VkDescriptorPool normal_descriptor_pool = VK_NULL_HANDLE;
	std::vector< VkDescriptorSet > normal_descriptors; //allocated from texture_descriptor_pool

	//--------------------------------------------------------------------
	//Resources that change when the swapchain is resized:

	virtual void on_swapchain(RTG &, RTG::SwapchainEvent const &) override;

	Helpers::AllocatedImage swapchain_depth_image;
	VkImageView swapchain_depth_image_view = VK_NULL_HANDLE;
	std::vector< VkFramebuffer > swapchain_framebuffers;
	//used from on_swapchain and the destructor: (framebuffers are created in on_swapchain)
	void destroy_framebuffers();

	//--------------------------------------------------------------------
	//Resources that change when time passes or the user interacts:

	virtual void update(float dt) override;
	virtual void on_input(InputEvent const &input) override;

	virtual bool in_view(AABB box, mat4 transform);

	float time = 0.0f;

	mat4 CLIP_FROM_WORLD;
	mat4 DEBUG_CLIP_FROM_WORLD;

	//Camera modifiers
	bool mouse_down = false;
	float x0;
	float y0;

	std::vector< LinesPipeline::Vertex> lines_vertices;

	// ObjectsPipeline::World world;

	std::vector< Light > light_instances;

	struct ObjectInstance {
		ObjectVertices vertices;
		ObjectsPipeline::Transform transform;
		uint32_t texture = 0;
		uint32_t normal = 0;	
	};
	std::vector< ObjectInstance > object_instances;

	//--------------------------------------------------------------------
	//Rendering function, uses all the resources above to queue work to draw a frame:

	virtual void render(RTG &, RTG::RenderParams const &) override;
};


#endif /* DC95ECE2_4E01_4EFA_8058_92A36917FC0C */


#endif /* AA32B710_B7A4_48B9_AC32_AF4954B60C38 */
