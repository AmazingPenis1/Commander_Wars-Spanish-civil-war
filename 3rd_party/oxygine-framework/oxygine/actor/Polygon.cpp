#include "Polygon.h"
#include "../RenderDelegate.h"
#include "../RenderState.h"
#include "../STDRenderDelegate.h"
#include "../STDRenderer.h"
#include "../core/UberShaderProgram.h"
#include "../core/VertexDeclaration.h"
#include <sstream>

namespace oxygine
{
    Polygon::Polygon()
        : _vdecl(0),
          _own(false),
          _verticesData(0),
          _verticesSize(0)

    {

    }

    Polygon::~Polygon()
    {
        if (_own)
            delete[] _verticesData;
    }

    void Polygon::copyFrom(const Polygon& src, cloneOptions opt)
    {
        Sprite::copyFrom(src, opt);
    }

    void Polygon::setVertices(const void* data, int size, int bformat, bool own)
    {
        if (_own && data != _verticesData)
        {
            delete[] _verticesData;
        }

        _own = own;
        _verticesData = reinterpret_cast<const unsigned char*>(data);
        _verticesSize = size;
        _vdecl = IVideoDriver::instance->getVertexDeclaration(bformat);
    }

    template <class T>
    void append(std::vector<unsigned char>& buff, const T& t)
    {
        const unsigned char* ptr = (const unsigned char*)&t;
        buff.insert(buff.end(), ptr, ptr + sizeof(t));
    }

    void Polygon::doRender(const RenderState& rs)
    {
        if (!_verticesSize)
            return;



        _mat->apply2([ = ]()
        {

            STDRenderer* renderer = STDRenderer::getCurrent();

            static std::vector<unsigned char> buff;
            buff.clear();

            buff.reserve(_verticesSize);
            int num = _verticesSize / _vdecl->size;

            const unsigned char* ptr = (const unsigned char*)_verticesData;
            for (int i = 0; i < num; ++i)
            {
                const Vector2* pos = (Vector2*)ptr;
                Vector2 t = rs.transform.transform(*pos);

                append(buff, t);
                buff.insert(buff.end(), ptr + sizeof(t), ptr + sizeof(t) + _vdecl->size - sizeof(t));

                ptr += _vdecl->size;
            }

            renderer->addVertices(&buff.front(), (unsigned int)buff.size());

        });
    }
}
