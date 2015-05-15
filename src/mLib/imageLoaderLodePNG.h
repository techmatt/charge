#ifndef _EXT_LODEPNG_IMAGELOADERLODEPNG_H_
#define _EXT_LODEPNG_IMAGELOADERLODEPNG_H_

class LodePNG
{
public:
  static Bitmap load(const std::string &filename);
  static void save(const Bitmap &image, const std::string &filename);
};

#endif  // _EXT_LODEPNG_IMAGELOADERLODEPNG_H_
