#include "vulkan_core/handlers/PipelineHandler.hpp"
#include "vulkan_core/handlers/DeviceHandler.hpp"
#include "vulkan_core/handlers/SwapChainHandler.hpp"
#include "vulkan_core/utils/VulkanUtils.hpp"
#include <stdexcept>

namespace Accretion{

PipelineHandler::PipelineHandler() : VulkanHandler("PipelineHandler", [&]{
  if(m_pipeline)
    vkDestroyPipeline(get_dependency<DeviceHandler>().get_device(), m_pipeline, nullptr);
  if(m_pipeline_layout)
    vkDestroyPipelineLayout(get_dependency<DeviceHandler>().get_device(), m_pipeline_layout, nullptr);
}){
  request_dependency("SwapChainHandler");
  request_dependency("DeviceHandler");
}

void PipelineHandler::create(){
  VkDevice& device = get_dependency<DeviceHandler>().get_device();
  VkExtent2D extent = get_dependency<SwapChainHandler>().get_extent();

  auto vert_code = ReadFileUint32("shaders/triangle.vert.spv");
  auto frag_code = ReadFileUint32("shaders/triangle.frag.spv");

  VkShaderModuleCreateInfo vert_module_info{};
  vert_module_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  vert_module_info.codeSize = vert_code.size() * sizeof(uint32_t);
  vert_module_info.pCode = vert_code.data();

  VkShaderModule vert_module{};
  if(vkCreateShaderModule(device, &vert_module_info, nullptr, &vert_module) != VK_SUCCESS)
    throw std::runtime_error("ERROR: Couldn't create the vertex Shader.");

  VkShaderModuleCreateInfo frag_module_info{};
  frag_module_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  frag_module_info.codeSize = frag_code.size() * sizeof(uint32_t);
  frag_module_info.pCode = frag_code.data();

  VkShaderModule frag_module{};
  if(vkCreateShaderModule(device, &frag_module_info, nullptr, &frag_module) != VK_SUCCESS)
    throw std::runtime_error("ERROR: Couldn't create a Shader.");

  VkPipelineShaderStageCreateInfo vert_stage_info{};
  vert_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vert_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
  vert_stage_info.module = vert_module;
  vert_stage_info.pName = "main";

  VkPipelineShaderStageCreateInfo frag_stage_info{};
  frag_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  frag_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  frag_stage_info.module = frag_module;
  frag_stage_info.pName = "main";

  VkPipelineShaderStageCreateInfo stages[2]{vert_stage_info, frag_stage_info};

  VkPipelineVertexInputStateCreateInfo vertex_input{};
  vertex_input.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

  VkPipelineInputAssemblyStateCreateInfo input_assembly{};
  input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  input_assembly.primitiveRestartEnable = VK_FALSE;

  VkViewport viewport{};
  viewport.width = extent.width;
  viewport.height = extent.height;
  viewport.x = 0.f;
  viewport.y = 0.f;
  viewport.minDepth = 0.f;
  viewport.maxDepth = 1.f;

  VkRect2D scissor{};
  scissor.extent = extent;
  scissor.offset = {0, 0};

  VkPipelineViewportStateCreateInfo viewport_state{};
  viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewport_state.viewportCount = 1;
  viewport_state.pViewports = &viewport;
  viewport_state.scissorCount = 1;
  viewport_state.pScissors = &scissor;

  VkPipelineRasterizationStateCreateInfo rasterization_state{};
  rasterization_state.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterization_state.depthClampEnable = VK_FALSE;
  rasterization_state.depthBiasEnable = VK_FALSE;
  rasterization_state.rasterizerDiscardEnable = VK_FALSE;
  rasterization_state.polygonMode = VK_POLYGON_MODE_FILL;
  rasterization_state.lineWidth = 1.f;
  rasterization_state.cullMode = VK_CULL_MODE_NONE;
  rasterization_state.frontFace = VK_FRONT_FACE_CLOCKWISE;

  VkPipelineMultisampleStateCreateInfo multisampling_state{};
  multisampling_state.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampling_state.sampleShadingEnable = VK_FALSE;
  multisampling_state.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
  multisampling_state.minSampleShading = 1.f;
  multisampling_state.alphaToCoverageEnable = VK_FALSE;
  multisampling_state.alphaToOneEnable = VK_FALSE;

  VkPipelineRenderingCreateInfo rendering_info{};
  rendering_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
  rendering_info.viewMask = 0;
  rendering_info.colorAttachmentCount = 1;
  VkFormat color_format = VK_FORMAT_R8G8B8A8_UNORM;
  rendering_info.pColorAttachmentFormats = &color_format;
  rendering_info.depthAttachmentFormat = VK_FORMAT_D32_SFLOAT;
  rendering_info.stencilAttachmentFormat = VK_FORMAT_S8_UINT;

  VkPipelineColorBlendAttachmentState color_blend_attachment{};
  color_blend_attachment.colorWriteMask = 
    VK_COLOR_COMPONENT_R_BIT |
    VK_COLOR_COMPONENT_G_BIT |
    VK_COLOR_COMPONENT_B_BIT |
    VK_COLOR_COMPONENT_A_BIT;
  color_blend_attachment.blendEnable = VK_TRUE;
  color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
  color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
  color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
  color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
  color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
  color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;

  VkPipelineColorBlendStateCreateInfo color_blending{};
  color_blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  color_blending.logicOpEnable = VK_FALSE;
  color_blending.logicOp = VK_LOGIC_OP_COPY;
  color_blending.attachmentCount = 1;
  color_blending.pAttachments = &color_blend_attachment;
  color_blending.blendConstants[0] = 0.0f;
  color_blending.blendConstants[1] = 0.0f;
  color_blending.blendConstants[2] = 0.0f;
  color_blending.blendConstants[3] = 0.0f; 

  std::vector<VkDynamicState> dynamic_states = {
    VK_DYNAMIC_STATE_VIEWPORT,
    VK_DYNAMIC_STATE_SCISSOR
  };

  VkPipelineDynamicStateCreateInfo dynamic_state{};
  dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamic_state.dynamicStateCount = static_cast<uint32_t>(dynamic_states.size());
  dynamic_state.pDynamicStates = dynamic_states.data();

  VkPipelineDepthStencilStateCreateInfo depth_stencil{};
  depth_stencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  depth_stencil.depthTestEnable = VK_TRUE;
  depth_stencil.depthWriteEnable = VK_TRUE;
  depth_stencil.depthCompareOp = VK_COMPARE_OP_LESS;
  depth_stencil.depthBoundsTestEnable = VK_FALSE;
  depth_stencil.stencilTestEnable = VK_FALSE;


  VkPipelineLayoutCreateInfo layout_info{};
  layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

  if(vkCreatePipelineLayout(device, &layout_info, nullptr, &m_pipeline_layout) != VK_SUCCESS)
    throw std::runtime_error("ERROR: Failed to create Pipeline Layout.");

  VkGraphicsPipelineCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  create_info.pNext = &rendering_info;
  create_info.stageCount = 2;
  create_info.pStages = stages;
  create_info.pVertexInputState = &vertex_input;
  create_info.pInputAssemblyState = &input_assembly;
  create_info.pViewportState = &viewport_state;
  create_info.pRasterizationState = &rasterization_state;
  create_info.pMultisampleState = &multisampling_state;
  create_info.pColorBlendState = &color_blending;
  create_info.pDynamicState = &dynamic_state;
  create_info.pDepthStencilState = &depth_stencil;
  create_info.layout = m_pipeline_layout;
  create_info.renderPass = VK_NULL_HANDLE;
  create_info.subpass = 0;

  VkResult result = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &create_info, nullptr, &m_pipeline);
  if(result != VK_SUCCESS)
    throw std::runtime_error("FATAL ERROR: Failed to create Graphics Pipeline.");
  vkDestroyShaderModule(device, vert_module, nullptr);
  vkDestroyShaderModule(device, frag_module, nullptr);
}

}
