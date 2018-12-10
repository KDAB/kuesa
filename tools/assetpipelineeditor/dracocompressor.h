#ifndef DRACOCOMPRESSOR_H
#define DRACOCOMPRESSOR_H

#include <memory>
#include <draco/compression/encode.h>

namespace Qt3DRender
{
class QGeometry;
};

std::unique_ptr<draco::EncoderBuffer> compressMesh(const Qt3DRender::QGeometry &geometry);

#endif // DRACOCOMPRESSOR_H
