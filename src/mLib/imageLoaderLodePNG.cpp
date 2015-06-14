
#include "main.h"

#include "lodepng.h"

Bitmap LodePNG::load(const std::string &filename)
{
    if (!util::fileExists(filename))
    {
        std::cout << "LodePNG::load file not found: " << filename << std::endl;
        return Bitmap();
    }

    std::vector<BYTE> image;
    UINT width, height;

    UINT error = lodepng::decode(image, width, height, filename);

    MLIB_ASSERT_STR(!error, std::string(lodepng_error_text(error)) + ": " + filename);

    Bitmap result;

    if (!error)
    {
        result.allocate(width, height);
        memcpy(result.data(), &image[0], 4 * width * height);
    }

    return result;
}

Bitmap LodePNG::load(const vector<BYTE> &data)
{
    std::vector<BYTE> image;
    UINT width, height;

    UINT error = lodepng::decode(image, width, height, data);

    MLIB_ASSERT_STR(!error, std::string(lodepng_error_text(error)));

    Bitmap result;

    if (!error)
    {
        result.allocate(width, height);
        memcpy(result.data(), &image[0], 4 * width * height);
    }

    return result;
}

void LodePNG::save(const Bitmap &image, const std::string &filename)
{
    const size_t pixelCount = image.width() * image.height();

    //
    // images should be saved with no transparency, which unfortunately requires us to make a copy of the bitmap data.
    //
    RGBColor *copy = new RGBColor[pixelCount];
    memcpy(copy, image.data(), pixelCount * 4);
    for (UINT i = 0; i < pixelCount; i++)
        copy[i].a = 255;

    lodepng::encode(filename, (const BYTE *)copy, (unsigned int)image.width(), (unsigned int)image.height(), LodePNGColorType::LCT_RGBA);
    delete[] copy;
}
