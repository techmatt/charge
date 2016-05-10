
class D3D11Utility
{
public:
	static ID3DBlob* CompileShader(
    const string &filename, 
    const string &entryPoint, 
    const string &shaderModel, 
    const vector<pair<string, string>>& macros = vector<pair<string, string>>());
};
