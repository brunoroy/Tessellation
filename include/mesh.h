#ifndef MESH_H
#define MESH_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <QString>
#include <QStringList>
#include <boost/unordered_map.hpp>
#include "glm/ext.hpp"
#include "material.h"

struct OBJVertex {
    uint32_t p, n, uv;

    inline OBJVertex() { }

    OBJVertex(const QString &string)
        : n((uint32_t) -1), uv((uint32_t) -1) {
        QStringList tokens = string.split("/");

        bool ok;
        p  = (uint) tokens[0].toInt(&ok) - 1;

        if (tokens.size() == 3) {
            if (tokens[1].length() > 0) {
                uv = (uint32_t) tokens[1].toInt(&ok) - 1;
            }
            if (tokens[2].length() > 0) {
                n  = (uint32_t) tokens[2].toInt(&ok) - 1;
            }
        }

        return;
    }

    inline bool operator==(const OBJVertex &v) const {
        return v.p == p && v.n == n && v.uv == uv;
    }
};

struct OBJVertexHash : std::unary_function<OBJVertex, size_t> {
    std::size_t operator()(const OBJVertex &v) const {
        size_t hash = 0;
        boost::hash_combine(hash, v.p);
        boost::hash_combine(hash, v.n);
        boost::hash_combine(hash, v.uv);
        return hash;
    }
};

struct Vertex
{
public:
    Vertex(glm::vec3 position, glm::vec2 textureCoordinate, glm::vec3 normal):
        _position(position), _textureCoordinate(textureCoordinate), _normal(normal) {}

    glm::vec3 getPosition() {return _position;}
    glm::vec3 getNormal() {return _normal;}
    glm::vec2 getTextureCoordinate() {return _textureCoordinate;}

private:
    glm::vec3 _position;
    glm::vec2 _textureCoordinate;
    glm::vec3 _normal;
};

struct IndicePolygon
{
    IndicePolygon(uint vertex, uint uv, uint normal):
        vertex(vertex), uv(uv), normal(normal) {}

    uint vertex;
    uint uv;
    uint normal;
};

struct Polygons
{
    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> textureCoordinates;
    std::vector<glm::vec3> normals;

    size_t getSize() {return positions.size();}
};

struct Polygon
{
    glm::vec3 positions[3];
    glm::vec2 textureCoordinates[3];
    glm::vec3 normals[3];
};

class Mesh
{
public:
    Mesh();
    Mesh(Mesh* mesh);
    Mesh(QString filename, bool invertNormals = false);
    ~Mesh();

    bool loadObject(QString filename, bool invertNormals);
    void addVertex(Vertex vertex)
    {
        _positions.push_back(vertex.getPosition());
        _normals.push_back(vertex.getNormal());
        _textureCoordinates.push_back(vertex.getTextureCoordinate());
        _vertices.push_back(vertex);
    }

    std::vector<Vertex> getVertices() {return _vertices;}
    std::vector<uint> getIndices() {return _indices;}
    std::vector<glm::vec3> getPositions() {return _positions;}
    std::vector<glm::vec3> getNormals() {return _normals;}
    std::vector<glm::vec2> getTextureCoordinates() {return _textureCoordinates;}
    void setMVP(glm::mat4 matrix);

    void translate(glm::vec3 vector){_translation = glm::translate(_translation, vector);}
    void rotate(float angle, glm::vec3 vector) {_rotation = glm::rotate(_rotation, angle, vector);}
    void scale(glm::vec3 vector) {_scaling = glm::scale(_scaling, vector);}
    glm::mat4 getModelMatrix() {return _translation * _rotation * _scaling;}

    bool isObjModel() {return _objModel;}

    void initialize();
    void preDraw();
    void draw();

    Shader* getShader() {return _material->getShader();}
    Material* getMaterial() {return _material;}

    bool isQuads() {return _indices.size()%3 == 0;}
    bool isTriangles() {return _indices.size()%4 == 0;}

protected:
    std::vector<uint> _indices;
    std::vector<IndicePolygon> _indicePolygons;
    std::vector<glm::vec3> _positions;
    //std::vector<glm::vec4> _colors;
    std::vector<glm::vec3> _normals;
    std::vector<glm::vec2> _textureCoordinates;

    uint *_indiceArray;
    float *_positionArray;
    float *_textureArray;
    float *_normalArray;

    std::vector<Polygon> _polygons;
    std::vector<Vertex> _vertices;

    Material *_material;

private:
    bool _hasNormals;
    bool _objModel;

    uint _locationVertices;
    uint _locationTextureCoordinates;
    uint _locationNormals;

    uint _triangleCount;
    uint _vertexCount;

    glm::mat4 _translation;
    glm::mat4 _rotation;
    glm::mat4 _scaling;

    glm::mat4 _mvp;

    GLuint _vertexArrayId;
    GLuint _indiceBuffer;
    GLuint _vertexBuffer;
    GLuint _colorBuffer;
    GLuint _textureBuffer;
    GLuint _normalBuffer;

    bool _invertNormals;
};

#endif // MESH_H
