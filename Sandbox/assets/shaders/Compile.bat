@ECHO OFF
D:\VulkanSDK\1.3.250.1\Bin\glslc.exe -fshader-stage=vertex VertexShader.glsl -o VertexShader.spv
D:\VulkanSDK\1.3.250.1\Bin\glslc.exe -fshader-stage=fragment FragmentShader.glsl -o FragmentShader.spv
PAUSE