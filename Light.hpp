#ifndef CC4ECD8A_DE57_4455_9765_1E8BB899200A
#define CC4ECD8A_DE57_4455_9765_1E8BB899200A
#include <vulkan/vulkan_core.h>

#include <cstdint>

struct Light {
    struct { float x, y, z , w;} Tint;
    struct { float x, y, z , w;} Pos;
    struct { float x, y, z , w;} RLB;
    struct { float x, y, z , w;} Dir;
    // struct { float x;} StrPow;
    // struct { float x, y, z;} Pos;
    // struct { float x;} AngFov;
    // struct { float r, l, b;} RLB;
    // struct { float t;} Type;
    // struct { float x;} Radius;
    // struct { float x;} Limit;
    // struct { float x;} Blend;
    // struct { uint_t m;} Shadow;
    //a pipeline vertex input state that works with a buffer holding a Light[] array:
    static const VkPipelineVertexInputStateCreateInfo array_input_state;
};

enum LightType {
    SUN = 0,
    SPHERE = 1,
    SPOT = 2,
};

// static_assert(sizeof(Light) == 3*4 + 5*4 + 4*4 + 4*4 , "Light is packed.");
static_assert(sizeof(Light) == 4*4 + 4*4 + 4*4 + 4*4 , "Light is packed.");



#endif /* CC4ECD8A_DE57_4455_9765_1E8BB899200A */
