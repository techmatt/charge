
#include "main.h"

#ifdef __APPLE__
#import <CoreFoundation/CoreFoundation.h>
#include <unistd.h>
#endif

GameParameters g_gameParams;

void chargeFatalError(const string &text)
{
    cout << "Fatal error: " << text << endl;
    cin.get();
    exit(1);
}

void initGameParams(const ParameterFile &params)
{
    g_gameParams.load(params);
    g_gameParams.database = new Database();
    g_gameParams.database->init();
}

/*int binarySearch(const vector<float> &v, float target, int low = 0, int high = std::numeric_limits<int>::max())
{
    high = std::min(high, (int)v.size() - 1);
    if (abs(high - low) <= 1)
        return low;
    int mid = (high + low) / 2;
    if (v[mid] == target)
        return mid;
    else if (v[mid] > target)
        return binarySearch(v, target, low, mid);
    else
        return binarySearch(v, target, mid + 1, high);
}

void outputLHistogram(const Bitmap &bmp, const string &filename)
{
    ofstream file(filename);
    auto L = [](const vec3f &v)
    {
        //return (v.x + v.y + v.z) / 3.0f;
        return 0.3 * v.x + 0.5 * v.y + 0.2 * v.z;
    };
    vector<float> sourceLValues;
    for (auto &p : bmp)
    {
        const vec3f s = bmp(p.x, p.y).toVec3f();
        const float sourceL = L(s);
        sourceLValues.push_back(sourceL);
    }
    sort(sourceLValues.begin(), sourceLValues.end());
    for (auto f : sourceLValues)
    {
        file << f << endl;
    }
}

Bitmap normalizeColors(const Bitmap &bmpSource, const Bitmap &bmpTarget)
{
    auto L = [](const vec3f &v)
    {
        //return (v.x + v.y + v.z) / 3.0f;
        return 0.3 * v.x + 0.5 * v.y + 0.2 * v.z;
    };
    Bitmap result = bmpSource;
    vector<float> sourceLValues;
    vector<float> targetLValues;
    for (auto &p : result)
    {
        const vec3f s = bmpSource(p.x, p.y).toVec3f();
        const vec3f t = bmpTarget(p.x, p.y).toVec3f();
        const float sourceL = L(s);
        const float targetL = L(t);
        sourceLValues.push_back(sourceL);
        targetLValues.push_back(targetL);
    }
    sort(sourceLValues.begin(), sourceLValues.end());
    sort(targetLValues.begin(), targetLValues.end());

    for (auto &p : result)
    {
        vec3f s = bmpSource(p.x, p.y).toVec3f();
        const float sourceL = L(s);
        int sourceHistorgramIndex = binarySearch(sourceLValues, sourceL);
        const float sourceLFound = sourceLValues[sourceHistorgramIndex];
        if (sourceL > 0.9f)
        {
            int a = 5;
        }
        if (fabs(sourceLFound - sourceL) > 0.01f)
        {
            cout << "mismatchA" << endl;
        }
        const float targetL = targetLValues[sourceHistorgramIndex];
        const float scale = targetL / max(sourceL, 0.001f);
        //const float scale = 1.0f;
        const float newL = L(s * scale);
        const vec3f cOut = s * 255.0f * scale;
        p.value.r = util::boundToByte(cOut.r);
        p.value.g = util::boundToByte(cOut.g);
        p.value.b = util::boundToByte(cOut.b);
    }

    return result;
}

void processVideo()
{
    string baseDir = "G:/StyleTransfer/";
    Bitmap bmpStyle = LodePNG::load(baseDir + "picasso.png");
    Bitmap bmpContent = LodePNG::load(baseDir + "clipA.png");

    bmpStyle.allocate(10, 1, RGBColor(0, 0, 0));
    for (auto &p : bmpStyle)
        p.value.g = util::boundToByte(p.x * 10);
    bmpContent.allocate(10, 1, RGBColor(0, 0, 0));
    for (auto &p : bmpContent)
        p.value.g = util::boundToByte(p.x * 10 + 100);

    Bitmap bmpNormalized = normalizeColors(bmpStyle, bmpContent);

    outputLHistogram(bmpStyle, baseDir + "style.csv");
    outputLHistogram(bmpContent, baseDir + "content.csv");
    outputLHistogram(bmpNormalized, baseDir + "noramlized.csv");

    LodePNG::save(bmpNormalized, baseDir + "clipAOut.png");
}*/

void makeAffinityImage(const vec3f &baseColor, int size, const string &baseOut, float baseAlpha, bool big)
{
    float innerRadius, middleRadius, outerRadius;
    if (big)
    {
        innerRadius = 0.3f;
        middleRadius = 0.38f;
        outerRadius = 0.45f;
    }
    else
    {
        innerRadius = 0.3f;
        middleRadius = 0.33f;
        outerRadius = 0.41f;
    }
    Bitmap bmpColor(size, size);
    Bitmap bmpAlpha(size, size);
    for (auto &p : bmpColor)
    {
        const float dist = vec2f::dist(vec2f(p.x, p.y) / size, vec2f(0.5f, 0.5f));
        if (dist <= innerRadius)
        {
            bmpColor(p.x, p.y) = RGBColor(vec3f(0.0f, 0.0f, 0.0f));
            bmpAlpha(p.x, p.y) = RGBColor(vec3f(0.0f));
        }
        else if (dist <= middleRadius)
        {
            bmpColor(p.x, p.y) = RGBColor(baseColor);
            bmpAlpha(p.x, p.y) = RGBColor(vec3f(baseAlpha));
        }
        else if (dist >= outerRadius)
        {
            bmpColor(p.x, p.y) = RGBColor(vec3f(1.0f, 1.0f, 1.0f));
            bmpAlpha(p.x, p.y) = RGBColor(vec3f(0.0f, 0.0f, 0.0f));
        }
        else
        {
            const float s = math::linearMap(middleRadius, outerRadius, 0.0f, 1.0f, dist);
            bmpColor(p.x, p.y) = math::lerp(baseColor, vec3f(1.0f, 1.0f, 1.0f), s);
            bmpAlpha(p.x, p.y) = RGBColor(vec3f(math::lerp(baseAlpha, 0.5f, s)));
        }
    }
    LodePNG::save(bmpColor, baseOut + ".png");
    LodePNG::save(bmpAlpha, baseOut + "Alpha.png");
}

void makeAffinityStuff()
{
    makeAffinityImage(vec3f(0.95f, 0.25f, 0.25f), 512, R"(C:\Code\Charge\temp\PreferenceMask0)", 0.95f, true);
    makeAffinityImage(vec3f(0.95f, 0.5f, 0.5f), 512, R"(C:\Code\Charge\temp\PreferenceMask1)", 0.95f, false);
    makeAffinityImage(vec3f(0.5f, 0.95f, 0.5f), 512, R"(C:\Code\Charge\temp\PreferenceMask3)", 0.95f, false);
    makeAffinityImage(vec3f(0.25f, 0.95f, 0.25f), 512, R"(C:\Code\Charge\temp\PreferenceMask4)", 0.95f, true);
}

int main(int argc, char* argv[])
{
    makeAffinityStuff();
    //processVideo();
#ifdef __APPLE__
    CFBundleRef mainBundle = CFBundleGetMainBundle();
    CFURLRef resourcesURL = CFBundleCopyPrivateFrameworksURL(mainBundle);
    char path[PATH_MAX];
    if (!CFURLGetFileSystemRepresentation(resourcesURL, TRUE, (UInt8 *)path, PATH_MAX)) {
        return 1;
    }
    chdir(path);
#endif

    string paramFilename = "../assets/parameters.txt";
    
    ParameterFile parameterFile(paramFilename);
    initGameParams(parameterFile);

    Renderer *renderer = nullptr;

    if (params().useSDLFallback)
        renderer = new RendererSDL();
    else if (params().graphics == "OpenGL")
        renderer = new RendererOpenGL();
	else if (params().graphics == "D3D11")
	{
#ifdef INCLUDE_D3D
		renderer = new RendererD3D11();
#endif
	}
    else
        chargeFatalError("Unrecognized graphics type: " + params().graphics);

    App app(*renderer);
    return app.run();
    //return app.runRendererTest();
}
