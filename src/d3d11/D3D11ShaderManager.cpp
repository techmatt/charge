
#include "main.h"

void D3D11ShaderManager::init(RendererD3D11 &g)
{
    _graphics = &g;
}

void D3D11ShaderManager::registerShader(
  const string&filename, 
  const string& shaderName, 
  const string& entryPointVS, 
  const string& shaderModelVS, 
  const string& entryPointPS,
  const string& shaderModelPS
  )
{
    // in case the shader exists return
	if (_shaders.count(shaderName) == 0)
	{
		auto &shaders = _shaders[shaderName];
		shaders.vs.load(*_graphics, filename, entryPointVS, shaderModelVS);
		shaders.ps.load(*_graphics, filename, entryPointPS, shaderModelPS);
	}
}
