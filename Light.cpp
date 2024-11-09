#include "Light.hpp"

#include <array>

static std::array< VkVertexInputBindingDescription, 1 > bindings{
    VkVertexInputBindingDescription{
        .binding = 0,
        .stride = sizeof(Light),
        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
    }
};

static std::array< VkVertexInputAttributeDescription, 4> attributes {
    VkVertexInputAttributeDescription{
        .location = 0,
        .binding = 0,
        .format = VK_FORMAT_R32G32B32A32_SFLOAT,
        .offset = offsetof(Light, Tint),
    },
    // VkVertexInputAttributeDescription{
    //     .location = 1,
    //     .binding = 0,
    //     .format = VK_FORMAT_R32_SFLOAT,
    //     .offset = offsetof(Light, StrPow),
    // },
    VkVertexInputAttributeDescription{
        .location = 1,
        .binding = 0,
        .format = VK_FORMAT_R32G32B32A32_SFLOAT,
        .offset = offsetof(Light, Pos),
    },
    VkVertexInputAttributeDescription{
        .location = 2,
        .binding = 0,
        .format = VK_FORMAT_R32G32B32A32_SFLOAT,
        .offset = offsetof(Light, RLB),
    },
    VkVertexInputAttributeDescription{
        .location = 3,
        .binding = 0,
        .format = VK_FORMAT_R32G32B32_SFLOAT,
        .offset = offsetof(Light, Dir),
    },
    // VkVertexInputAttributeDescription{
    //     .location = 5,
    //     .binding = 0,
    //     .format = VK_FORMAT_R32_SFLOAT ,
    //     .offset = offsetof(Light, Type),
    // },
    
    // VkVertexInputAttributeDescription{
    //     .location = 6,
    //     .binding = 0,
    //     .format = VK_FORMAT_R32_SFLOAT,
    //     .offset = offsetof(Light, Limit),
    // },
    // VkVertexInputAttributeDescription{
    //     .location = 7,
    //     .binding = 0,
    //     .format = VK_FORMAT_R32_SFLOAT,
    //     .offset = offsetof(Light, Blend),
    // },
    // VkVertexInputAttributeDescription{
    //     .location = 7,
    //     .binding = 0,
    //     .format = VK_FORMAT_R32_UINT ,
    //     .offset = offsetof(Light, Shadow),
    // },
    // VkVertexInputAttributeDescription{
    //     .location = 8,
    //     .binding = 0,
    //     .format = VK_FORMAT_R32_UINT ,
    //     .offset = offsetof(Light, Type),
    // },
};

const VkPipelineVertexInputStateCreateInfo Light::array_input_state{
    .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
    .vertexBindingDescriptionCount = uint32_t(bindings.size()),
    .pVertexBindingDescriptions = bindings.data(),
    .vertexAttributeDescriptionCount = uint32_t(attributes.size()),
    .pVertexAttributeDescriptions = attributes.data(),
};