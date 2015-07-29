#include <GL/glew.h>
#include "mesh.h"

#include <QList>
#include <QFile>
#include <QTextStream>

#include <iostream>
#include <glm/glm.hpp>

Mesh::Mesh():
    _translation(1.0f),
    _rotation(1.0f),
    _scaling(1.0f),
    _objModel(false),
    _mvp(glm::mat4(1.0f))
{
}

Mesh::Mesh(Mesh *mesh)
{
    *this = mesh;
}

Mesh::Mesh(QString filename)
{
    if (!filename.isEmpty())
    {
        loadObject(filename);
        _objModel = true;
    }
}

Mesh::~Mesh()
{
    glDeleteBuffers(1, &_vertexBuffer);
    glDeleteBuffers(1, &_indiceBuffer);
}

void Mesh::initialize()
{
    //indices
    glGenBuffers(1, &_indiceBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indiceBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(uint), &_indices[0], GL_STATIC_DRAW);
    glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

    //vertices
    _locationVertices = _material->getShader()->getAttribute("position");
    glGenBuffers(1, &_vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, _positions.size() * sizeof(glm::vec3), &_positions[0], GL_STATIC_DRAW);
    glUnmapBuffer(GL_ARRAY_BUFFER);

    if(_textureCoordinates.size() > 0)
    {
        //texture
        _locationTextureCoordinates = _material->getShader()->getAttribute("uv");
        glGenBuffers(1, &_textureBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, _textureBuffer);
        glBufferData(GL_ARRAY_BUFFER, _textureCoordinates.size() * sizeof(glm::vec2), &_textureCoordinates[0], GL_STATIC_DRAW);
        glUnmapBuffer(GL_ARRAY_BUFFER);
    }

    if(_normals.size() > 0)
    {
        //normals
        _locationNormals = _material->getShader()->getAttribute("normal");
        glGenBuffers(1, &_normalBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, _normalBuffer);
        glBufferData(GL_ARRAY_BUFFER, _normals.size() * sizeof(glm::vec3), &_normals[0], GL_STATIC_DRAW);
        glUnmapBuffer(GL_ARRAY_BUFFER);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::preDraw()
{
    _material->bind();
}

void Mesh::setMVP(glm::mat4 matrix)
{
    _mvp =  matrix * getModelMatrix();
    Shader *shader = _material->getShader();
    shader->transmitUniform("mvp", _mvp);
}

void Mesh::draw()
{
    glEnableVertexAttribArray(_locationVertices);
    glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer);
    glVertexAttribPointer(_locationVertices, 3, GL_FLOAT, GL_FALSE, 0, 0);

    if (_textureCoordinates.size() > 0)
    {
        glEnableVertexAttribArray(_locationTextureCoordinates);
        glBindBuffer(GL_ARRAY_BUFFER, _textureBuffer);
        glVertexAttribPointer(_locationTextureCoordinates, 2, GL_FLOAT, GL_FALSE, 0, 0);
    }

    if (_normals.size() > 0)
    {
        glEnableVertexAttribArray(_locationNormals);
        glBindBuffer(GL_ARRAY_BUFFER, _normalBuffer);
        glVertexAttribPointer(_locationNormals, 3, GL_FLOAT, GL_FALSE, 0, 0);
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indiceBuffer);
    glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0);

    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);
}

//from Nori educational ray tracer
bool Mesh::loadObject(QString filename)
{
    typedef boost::unordered_map<OBJVertex, uint32_t, OBJVertexHash> VertexMap;

    QFile input(filename);
    if (!input.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QTextStream stream(&input);
    QTextStream line;
    QString temp, prefix;

    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> textureCoordinates;
    std::vector<OBJVertex> vertices;
    VertexMap vertexMap;

    uint triangles = 0;

    while (!(temp = stream.readLine()).isNull()) {
        line.setString(&temp);
        line >> prefix;
        if (prefix == "v") {
            glm::vec3 p;
            line >> p.x >> p.y >> p.z;
            positions.push_back(p);
        } else if (prefix == "vn") {
            glm::vec3 n;
            line >> n.x >> n.y >> n.z;
            normals.push_back(n);
        } else if (prefix == "vt") {
            glm::vec2 tc;
            line >> tc.x >> tc.y;
            textureCoordinates.push_back(tc);
        } else if (prefix == "f") {
            QString v1, v2, v3, v4;
            line >> v1 >> v2 >> v3 >> v4;
            OBJVertex tri[6];
            int nVertices = 3;

            tri[0] = OBJVertex(v1);
            tri[1] = OBJVertex(v2);
            tri[2] = OBJVertex(v3);
            triangles += 3;

            if (!v4.isNull()) {
                tri[3] = OBJVertex(v4);
                tri[4] = tri[0];
                tri[5] = tri[2];
                nVertices = 6;
                triangles += 3;
            }

            for (int i=0; i<nVertices; ++i) {
                const OBJVertex &v = tri[i];
                VertexMap::const_iterator it = vertexMap.find(v);
                if (it == vertexMap.end()) {
                    vertexMap[v] = (uint32_t) vertices.size();
                    _indicePolygons.push_back(IndicePolygon(v.p, v.uv, v.n));
                    vertices.push_back(v);
                } else {
                    _indicePolygons.push_back(IndicePolygon(v.p, v.uv, v.n));
                }
            }
        }
    }

    _triangleCount = (uint32_t) (_indicePolygons.size() / 3);
    _vertexCount = (uint32_t) positions.size();

    std::cout << "Read " << _triangleCount << " triangles and "
         << _vertexCount << " vertices." << std::endl;

    for (size_t i = 0; i < _indicePolygons.size(); i++)
    {
        _indices.push_back(i);
        _positions.push_back(positions.at(_indicePolygons.at(i).vertex));
        if (!textureCoordinates.empty())
            _textureCoordinates.push_back(textureCoordinates.at(_indicePolygons.at(i).uv));
        if (!normals.empty())
            _normals.push_back(normals.at(_indicePolygons.at(i).normal));
    }

    _material = new MaterialDefault(glm::vec4(1.0, 1.0, 1.0, 1.0));

    return true;
}
