set glslc_path=C:\VulkanSDK\1.3.231.1\Bin\glslc.exe
set batdir=%CD%
%glslc_path% %batdir%\..\Source\AliveLibAE\Renderer\Vulkan\shader.frag -o %batdir%\frag.spv
%glslc_path% %batdir%\..\Source\AliveLibAE\Renderer\Vulkan\shader.vert -o %batdir%\vert.spv

python bin2header.py -i %batdir%\frag.spv -o ..\Source\AliveLibAE\Renderer\Vulkan\fragment_shader.h -v fragment_shader
python bin2header.py -i %batdir%\vert.spv -o ..\Source\AliveLibAE\Renderer\Vulkan\vertex_shader.h -v vertex_shader

pause
