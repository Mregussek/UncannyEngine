
#include "GlslShaderCompiler.h"
#include "UGraphicsEngine/Renderer/Vulkan/Utilities.h"
#include "UTools/Logger/Log.h"
#include <glslang/Include/ResourceLimits.h>


namespace uncanny::vulkan
{


static glslang_stage_t TranslateShaderStage(EShaderCompilerStage shaderCompilerStage);


struct glslang_shader_raii_wrapper
{

  explicit glslang_shader_raii_wrapper(const glslang_input_t* pInput)
  {
    pShader = glslang_shader_create(pInput);
  }

  ~glslang_shader_raii_wrapper()
  {
    glslang_shader_delete(pShader);
  }

  glslang_shader_t* pShader{ nullptr };

};


struct glslang_program_raii_wrapper
{

  glslang_program_raii_wrapper()
  {
    pProgram = glslang_program_create();
  }

  ~glslang_program_raii_wrapper()
  {
    glslang_program_delete(pProgram);
  }

  glslang_program_t* pProgram{ nullptr };

};



FGLSLShaderCompiler::FGLSLShaderCompiler(u32 targetVulkanVersion)
{
  switch (targetVulkanVersion)
  {
    case VK_API_VERSION_1_3:
      m_TargetVulkanVersion = GLSLANG_TARGET_VULKAN_1_3;
      break;
    case VK_API_VERSION_1_2:
      m_TargetVulkanVersion = GLSLANG_TARGET_VULKAN_1_2;
      break;
    case VK_API_VERSION_1_1:
      m_TargetVulkanVersion = GLSLANG_TARGET_VULKAN_1_1;
      break;
    default:
      m_TargetVulkanVersion = GLSLANG_TARGET_VULKAN_1_0;
      break;
  }
}


FGLSLShaderCompiler::~FGLSLShaderCompiler()
{
  glslang_finalize_process();
}


void FGLSLShaderCompiler::Initialize()
{
  glslang_initialize_process();
}


std::vector<u32> FGLSLShaderCompiler::Compile(const char* glslSource, EShaderCompilerStage stage) const
{
  glslang_stage_t shaderStage = TranslateShaderStage(stage);

  TBuiltInResource builtInResource{};
  const glslang_input_t input{
    .language = GLSLANG_SOURCE_GLSL,
    .stage = shaderStage,
    .client = GLSLANG_CLIENT_VULKAN,
    .client_version = m_TargetVulkanVersion,
    .target_language = GLSLANG_TARGET_SPV,
    .target_language_version = GLSLANG_TARGET_SPV_1_6,
    .code = glslSource,
    .default_version = 460,
    .default_profile = GLSLANG_NO_PROFILE,
    .force_default_version_and_profile = UFALSE,
    .forward_compatible = UFALSE,
    .messages = GLSLANG_MSG_DEFAULT_BIT,
    .resource = reinterpret_cast<const glslang_resource_t*>(&builtInResource)
  };

  glslang_shader_raii_wrapper shaderRaiiWrapper(&input);
  glslang_program_raii_wrapper programRaiiWrapper{};

  b32 preprocessed = glslang_shader_preprocess(shaderRaiiWrapper.pShader, &input);
  if (not preprocessed)
  {
    UERROR("GLSL preprocessing failed:\n{}\n{}",
           glslang_shader_get_info_log(shaderRaiiWrapper.pShader),
           glslang_shader_get_info_debug_log(shaderRaiiWrapper.pShader));
    AssertVkAndThrow(VK_ERROR_INITIALIZATION_FAILED, "Cannot preprocess shader!");
  }

  b32 parsed = glslang_shader_parse(shaderRaiiWrapper.pShader, &input);
  if (not parsed)
  {
    UERROR("GLSL parsing failed:\n{}\n{}",
           glslang_shader_get_info_log(shaderRaiiWrapper.pShader),
           glslang_shader_get_info_debug_log(shaderRaiiWrapper.pShader),
           glslang_shader_get_preprocessed_code(shaderRaiiWrapper.pShader));
    AssertVkAndThrow(VK_ERROR_INITIALIZATION_FAILED, "Cannot parse shader!");
  }

  glslang_program_add_shader(programRaiiWrapper.pProgram, shaderRaiiWrapper.pShader);

  b32 linked = glslang_program_link(programRaiiWrapper.pProgram,
                                    GLSLANG_MSG_SPV_RULES_BIT | GLSLANG_MSG_VULKAN_RULES_BIT);
  if (not linked)
  {
    UERROR("GLSL linking failed:\n{}\n{}",
           glslang_program_get_info_log(programRaiiWrapper.pProgram),
           glslang_program_get_info_debug_log(programRaiiWrapper.pProgram));
    AssertVkAndThrow(VK_ERROR_INITIALIZATION_FAILED, "Cannot link program!");
  }

  glslang_program_SPIRV_generate(programRaiiWrapper.pProgram, shaderStage);
  std::vector<u32> spirvCode;
  spirvCode.resize(glslang_program_SPIRV_get_size(programRaiiWrapper.pProgram));
  glslang_program_SPIRV_get(programRaiiWrapper.pProgram, spirvCode.data());

  const char* spirvMessages = glslang_program_SPIRV_get_messages(programRaiiWrapper.pProgram);
  if (spirvMessages)
  {
    UERROR(spirvMessages);
  }

  return spirvCode;
}


glslang_stage_t TranslateShaderStage(EShaderCompilerStage shaderCompilerStage)
{
  switch(shaderCompilerStage)
  {
    case EShaderCompilerStage::VERTEX:
      return GLSLANG_STAGE_VERTEX;
    case EShaderCompilerStage::TESSCONTROL:
      return GLSLANG_STAGE_TESSCONTROL;
    case EShaderCompilerStage::TESSEVALUATION:
      return GLSLANG_STAGE_TESSEVALUATION;
    case EShaderCompilerStage::GEOMETRY:
      return GLSLANG_STAGE_GEOMETRY;
    case EShaderCompilerStage::FRAGMENT:
      return GLSLANG_STAGE_FRAGMENT;
    case EShaderCompilerStage::COMPUTE:
      return GLSLANG_STAGE_COMPUTE;
    case EShaderCompilerStage::RAYGEN:
      return GLSLANG_STAGE_RAYGEN;
    case EShaderCompilerStage::INTERSECT:
      return GLSLANG_STAGE_INTERSECT;
    case EShaderCompilerStage::ANYHIT:
      return GLSLANG_STAGE_ANYHIT;
    case EShaderCompilerStage::CLOSESTHIT:
      return GLSLANG_STAGE_CLOSESTHIT;
    case EShaderCompilerStage::MISS:
      return GLSLANG_STAGE_MISS;
  }

  AssertVkAndThrow(VK_ERROR_INITIALIZATION_FAILED, "Cannot translate shader stage!");
  return {};
}


}
