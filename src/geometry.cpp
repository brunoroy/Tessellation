#include <GL/glew.h>
#include "geometry.h"

#include <QList>
#include <QFile>
#include <QTextStream>

#include <iostream>
#include <fstream>
#include <glm/glm.hpp>

namespace Tessellation
{

    Geometry::Geometry():
        _translation(1.0f),
        _rotation(1.0f),
        _scaling(1.0f),
        _mvp(glm::mat4(1.0f)),
        _innerTL(1),
        _outerTL(1),
        _isTessellable(false),
        _id(0)
    {
    }

    Geometry::Geometry(Geometry *geometry)
    {
        *this = geometry;
    }

    Geometry::Geometry(QString filename, const uint id, const bool isTessellable)
    {
        std::string filetype = filename.mid(filename.length()-3, 3).toStdString();

        if (!filename.isEmpty())
        {
            if (filetype.compare("obj") == 0)
                loadModelWavefront(filename);
            else if (filetype.compare("ply") == 0)
                if (isTessellable)
                    loadModelPLY(filename);
                else
                    loadInputPoints(filename);
        }

        _id = id;
        _isTessellable = isTessellable;
    }

    Geometry::~Geometry()
    {
        glDeleteBuffers(1, &_vertexBuffer);
        glDeleteBuffers(1, &_indiceBuffer);
    }

    void Geometry::initialize()
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

        if(!_textureCoordinates.empty())
        {
            //texture
            _locationTextureCoordinates = _material->getShader()->getAttribute("uv");
            glGenBuffers(1, &_textureBuffer);
            glBindBuffer(GL_ARRAY_BUFFER, _textureBuffer);
            glBufferData(GL_ARRAY_BUFFER, _textureCoordinates.size() * sizeof(glm::vec2), &_textureCoordinates[0], GL_STATIC_DRAW);
            glUnmapBuffer(GL_ARRAY_BUFFER);
        }

        if(!_normals.empty())
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

    void Geometry::preDraw()
    {
        _material->bind();
        if (_isTessellable)
        {
            _material->getShader()->transmitUniform("innerTL", _innerTL);
            _material->getShader()->transmitUniform("outerTL", _outerTL);
        }
    }

    void Geometry::setMVP(glm::mat4 matrix)
    {
        _mvp =  matrix * getModelMatrix();
        _material->getShader()->transmitUniform("mvp", _mvp);
    }

    void Geometry::draw()
    {
        bool doTessellation = false;
        if (_isTessellable)
        {
            doTessellation = _material->getShader()->doTessellation();
            _material->getShader()->transmitUniform("doTessellation", doTessellation);
        }

        glEnableVertexAttribArray(_locationVertices);
        glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer);
        glVertexAttribPointer(_locationVertices, 3, GL_FLOAT, GL_FALSE, 0, 0);

        if (!_textureCoordinates.empty())
        {
            glEnableVertexAttribArray(_locationTextureCoordinates);
            glBindBuffer(GL_ARRAY_BUFFER, _textureBuffer);
            glVertexAttribPointer(_locationTextureCoordinates, 2, GL_FLOAT, GL_FALSE, 0, 0);
        }

        if (!_normals.empty())
        {
            glEnableVertexAttribArray(_locationNormals);
            glBindBuffer(GL_ARRAY_BUFFER, _normalBuffer);
            glVertexAttribPointer(_locationNormals, 3, GL_FLOAT, GL_FALSE, 0, 0);
        }

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indiceBuffer);
        if (_isTessellable) //mesh
        {
            if (doTessellation)
                glDrawArrays(GL_PATCHES, 0, _indices.size());
            else
                glDrawArrays(GL_TRIANGLES, 0, _indices.size());
        }
        else //point cloud
        {
            glPointSize(2.0f);
            glDrawArrays(GL_POINTS, 0, _indices.size());
            glPointSize(1.0f);
        }

        glDisableVertexAttribArray(2);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(0);
    }

    bool Geometry::loadModelPLY(QString filename)
    {
        std::string line;
        std::ifstream modelFile(filename.toStdString());
        if (modelFile.is_open())
        {
            QStringList properties;
            std::vector<glm::vec3> positions;
            std::vector<glm::vec3> normals;
            std::vector<glm::vec2> uvs;

            while (getline(modelFile, line))
            {
                QString value(line.c_str());
                if (value.contains("element vertex"))
                {
                    QStringList values = value.split(" ");
                    _vertexCount = values.at(2).toInt();
                }
                else if (value.contains("element face"))
                {
                    QStringList values = value.split(" ");
                    _triangleCount = values.at(2).toInt();
                }
                else if (value.contains("property float"))
                {
                    QStringList values = value.split(" ");
                    properties.push_back(values.at(2));
                }
                else if (value.compare("end_header") == 0)
                {
                    glm::vec3 position, normal;
                    for (int i = 0; i < _vertexCount; i++)
                    {
                        if (getline(modelFile, line))
                        {
                            QString value(line.c_str());
                            QStringList values = value.split(" ");
                            for (int p = 0; p < values.size(); p++)
                            {
                                if (properties.at(p).compare("x") == 0)
                                    position.x = values.at(p).toFloat();
                                else if (properties.at(p).compare("y") == 0)
                                    position.y = values.at(p).toFloat();
                                else if (properties.at(p).compare("z") == 0)
                                    position.z = values.at(p).toFloat();
                                else if (properties.at(p).compare("nx") == 0)
                                    normal.x = values.at(p).toFloat();
                                else if (properties.at(p).compare("ny") == 0)
                                    normal.y = values.at(p).toFloat();
                                else if (properties.at(p).compare("nz") == 0)
                                    normal.z = values.at(p).toFloat();
                            }

                            positions.push_back(position);
                            uvs.push_back(glm::vec2(1.0f, 1.0f));
                            normals.push_back(normal);
                        }
                    }

                    for (int i = 0; i < _triangleCount; i++)
                    {
                        if (getline(modelFile, line))
                        {
                            QString value(line.c_str());
                            QStringList values = value.split(" ");
                            int vertexCount = values.at(0).toInt();
                            if (vertexCount == 3)
                                for (int c = 0; c < vertexCount; c++)
                                    _indicePolygons.push_back(IndicePolygon(values.at(c+1).toInt(), values.at(c+1).toInt(), values.at(c+1).toInt()));

                        }
                    }
                }
            }

            for (size_t i = 0; i < _indicePolygons.size(); i++)
            {
                _indices.push_back(i);
                _positions.push_back(positions.at(_indicePolygons.at(i).vertex));
                if (!uvs.empty())
                    _textureCoordinates.push_back(uvs.at(_indicePolygons.at(i).uv));
                if (!normals.empty())
                    _normals.push_back(normals.at(_indicePolygons.at(i).normal));
            }

            std::cout << "Read " << _triangleCount << " triangles and "
                 << _vertexCount << " vertices." << std::endl;

            _material = new MaterialDefault(glm::vec4(1.0, 1.0, 1.0, 1.0));

            modelFile.close();
            return true;
        }
        else
            return false;
    }

    //from Nori educational ray tracer
    bool Geometry::loadModelWavefront(QString filename)
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

    bool Geometry::loadInputPoints(QString filename)
    {
        std::string line;
        std::ifstream inputFile(filename.toStdString());
        if (inputFile.is_open())
        {
            QStringList properties;

            while (getline(inputFile, line))
            {
                QString value(line.c_str());
                if (value.contains("element vertex"))
                {
                    QStringList values = value.split(" ");
                    _vertexCount = values.at(2).toInt();
                }
                else if (value.contains("property float"))
                {
                    QStringList values = value.split(" ");
                    properties.push_back(values.at(2));
                }
                else if (value.compare("end_header") == 0)
                {
                    glm::vec3 position, normal;
                    for (int i = 0; i < _vertexCount; i++)
                    {
                        if (getline(inputFile, line))
                        {
                            QString value(line.c_str());
                            QStringList values = value.split(" ");
                            for (int p = 0; p < values.size(); p++)
                            {
                                if (!values.at(p).isEmpty())
                                {
                                    if (properties.at(p).compare("x") == 0)
                                        position.x = values.at(p).toFloat();
                                    else if (properties.at(p).compare("y") == 0)
                                        position.y = values.at(p).toFloat();
                                    else if (properties.at(p).compare("z") == 0)
                                        position.z = values.at(p).toFloat();
                                    else if (properties.at(p).compare("nx") == 0)
                                        normal.x = values.at(p).toFloat();
                                    else if (properties.at(p).compare("ny") == 0)
                                        normal.y = values.at(p).toFloat();
                                    else if (properties.at(p).compare("nz") == 0)
                                        normal.z = values.at(p).toFloat();
                                }
                            }

                            _positions.push_back(position);
                            _indices.push_back(_positions.size()-1);
                        }
                    }
                }
            }

            std::cout << "Read " << _vertexCount << " points." << std::endl;

            inputFile.close();

            _material = new MaterialDefault(glm::vec4(1.0, 1.0, 1.0, 1.0));

            return true;
        }

        return false;
    }

    float GeometryTools::getDistance(Polygon polygon, glm::vec3 point)
    {
        glm::vec3 p0 = polygon.positions[0];
        glm::vec3 v1 = polygon.positions[1] - p0;
        glm::vec3 v2 = polygon.positions[2] - p0;

        glm::vec3 n = glm::cross(v1, v2);

        float sn = -glm::dot(n, point-p0);
        float sd = glm::dot(n, n);
        float d = sn / sd;

        return d;

        /*float    sb, sn, sd;

        sn = -dot( PL.n, (P - PL.V0));
        sd = dot(PL.n, PL.n);
        sb = sn / sd;

        *B = P + sb * PL.n;
        return d(P, *B);*/
    }

}
