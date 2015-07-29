#include <GL/glew.h>
#include "texture.h"
#include <QDir>
#include <iostream>

Texture::Texture(uint index)
{
    _height = 0;
    _width = 0;
    _hasImage = false;
    _format = GL_RGBA;
    _index = index;
    _unit = unitFromIndex(index);
    _id = 0;
    _hasImage = false;
    _minFilter = GL_LINEAR;
    _magFilter = GL_LINEAR;
    _globalFormat = GL_RGBA;
}

Texture::~Texture()
{
    glDeleteTextures(1, &_id);
}

void Texture::load(QString path)
{
    _image = QGLWidget::convertToGLFormat(QImage(path));
    _height = _image.height();
    _width = _image.width();
    _format = GL_RGBA;
    _hasImage = true;
}

void Texture::setupForFramebuffer(uint width, uint height, uint format, uint globalFormat)
{
    _width = width;
    _height = height;
    _format = format;
    _globalFormat = globalFormat;
    _minFilter = GL_LINEAR;
    _magFilter = GL_LINEAR;
}

void Texture::setFilters(uint min, uint mag)
{
    _minFilter = min;
    _magFilter = mag;
}

void Texture::initialize()
{
    glActiveTexture(_unit);
    glGenTextures(1, &_id);
    glBindTexture(GL_TEXTURE_2D, _id);

    float maxAnisotropy;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);

    if(_hasImage)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, _format, _width, _height, 0, _globalFormat, GL_UNSIGNED_BYTE, _image.bits());

        if(_minFilter == GL_LINEAR_MIPMAP_LINEAR)
            glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);
    }
    else
    {
        glTexImage2D(GL_TEXTURE_2D, 0, _format, _width, _height, 0, _globalFormat, GL_UNSIGNED_BYTE, NULL);
        if(_format == GL_DEPTH_COMPONENT)
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
            glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, _borderColor);
        }
        else
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
            glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, _borderColorB);
        }
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, _minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, _magFilter);
}

void Texture::bind()
{
    glActiveTexture(_unit);
    glBindTexture(GL_TEXTURE_2D, _id);
}

void Texture::resize(uint width, uint height)
{
    _width = width;
    _height = height;

    if(!_hasImage)
    {
        bind();
        glTexImage2D(GL_TEXTURE_2D, 0, _format, _width, _height, 0, _globalFormat, GL_UNSIGNED_BYTE, NULL);
    }
}

uint Texture::_unitCount = 0;
float Texture::_borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
float Texture::_borderColorB[] = {0.0f, 0.0f, 0.0f, 0.0f};

void Texture::resetUnit(int textureUnitOffset)
{
    _unitCount = textureUnitOffset;
}

Texture Texture::fromNextUnit()
{
    return Texture(_unitCount++);
}

Texture* Texture::newFromNextUnit()
{
    return new Texture(_unitCount++);
}

uint Texture::unitFromIndex(uint index)
{
    switch(index)
    {
        case 1: return GL_TEXTURE1;
        case 2: return GL_TEXTURE2;
        case 3: return GL_TEXTURE3;
        case 4: return GL_TEXTURE4;
        case 5: return GL_TEXTURE5;
        case 6: return GL_TEXTURE6;
        case 7: return GL_TEXTURE7;
        case 8: return GL_TEXTURE8;
        case 9: return GL_TEXTURE9;
        default: return GL_TEXTURE0;
    }
}

TextureCubeMap::TextureCubeMap(QString path):
    _id(0),
    _size(1024),
    _index(0)
{
    _unit = Texture::unitFromIndex(_index);
    load(path);
}

TextureCubeMap::~TextureCubeMap()
{
    _images.clear();
    glDeleteTextures(1, &_id);
}

void TextureCubeMap::initialize()
{
    glActiveTexture(_unit);
    glEnable(GL_TEXTURE_CUBE_MAP);
    glGenTextures(1, &_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, _id);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA, _size, _size, 0, GL_RGBA, GL_UNSIGNED_BYTE, _images.at(0).bits());
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA, _size, _size, 0, GL_RGBA, GL_UNSIGNED_BYTE, _images.at(1).bits());
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA, _size, _size, 0, GL_RGBA, GL_UNSIGNED_BYTE, _images.at(2).bits());
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA, _size, _size, 0, GL_RGBA, GL_UNSIGNED_BYTE, _images.at(3).bits());
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA, _size, _size, 0, GL_RGBA, GL_UNSIGNED_BYTE, _images.at(4).bits());
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA, _size, _size, 0, GL_RGBA, GL_UNSIGNED_BYTE, _images.at(5).bits());
}

void TextureCubeMap::bind()
{
    glActiveTexture(_unit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, _id);
}

void TextureCubeMap::load(QString path)
{
    QDir imageFolder(path);
    imageFolder.setNameFilters(QStringList("*.jpg"));
    imageFolder.setFilter(QDir::Files);
    QStringList imageFiles = imageFolder.entryList();

    foreach (QString imageFile, imageFiles)
    {
        QString filename = QString(imageFolder.path()).append("/").append(imageFile);
        _images.push_back(QGLWidget::convertToGLFormat(QImage(filename)));
    }
}

