#ifndef TEXTURE_H
#define TEXTURE_H

#include <QGLWidget>
#include <QDebug>
#include <QString>
#include <QImage>

class Texture
{
public:
    Texture(uint index);
    ~Texture();

    void initialize();
    void bind();
    void setupForFramebuffer(uint width, uint height, uint format, uint globalFormat = GL_RGBA);
    void load(QString path);
    void resize(uint width, uint height);

    void setFilters(uint min, uint mag);
    static void resetUnit(int textureUnitOffset = 0);
    static Texture fromNextUnit();
    static Texture* newFromNextUnit();
    static uint unitFromIndex(uint index);

    int getWidth() {return _width;}
    int getHeight() {return _height;}
    uint getIndex() {return _index;}

    uint _id;
    uint _unit;
    uint _index;

private:
    static uint _unitCount;
    static float _borderColor[];
    static float _borderColorB[];

    QImage _image;
    bool _hasImage;
    uint _height;
    uint _width;
    uint _minFilter;
    uint _magFilter;
    uint _format;
    uint _globalFormat;
};

class TextureCubeMap
{
public:
    TextureCubeMap(QString path);
    ~TextureCubeMap();

    void initialize();
    void bind();
    void load(QString path);

    GLuint _id;
    uint _index;
    uint _unit;

private:
    uint _size;
    std::vector<QImage> _images;
};

#endif // TEXTURE_H
