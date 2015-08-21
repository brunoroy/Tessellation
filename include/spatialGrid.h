#ifndef SPATIAL_GRID_H
#define SPATIAL_GRID_H

#include <vector>
#include <algorithm>
#include <string>
#include <sstream>

#include <glm/glm.hpp>

namespace Tessellation
{

    typedef unsigned int uint;

    struct Dimension
    {
        Dimension() {}
        Dimension(float min, float max):
            min(min), max(max)
        {
            length = fabs(min) + fabs(max);
        }

        float length;
        float min;
        float max;
    };

    class Volume
    {
    public:
        Volume(){}
        Volume(glm::vec3 volume):_width(volume.x), _height(volume.y), _depth(volume.z){}
        Volume(const float width, const float height, const float depth):
            _width(width), _height(height), _depth(depth){}

        float getWidth(){return _width;}
        float getHeight(){return _height;}
        float getDepth(){return _depth;}
        float getDimension(){return _width*_height*_depth;}

        void setWidth(const float width){_width = width;}
        void setHeight(const float height){_height = height;}
        void setDepth(const float depth){_depth = depth;}

    private:
        float _width;
        float _height;
        float _depth;
    };

    class Point
    {
    public:
        Point(){}
        Point(const int id, const glm::vec3 position):
            _id(id), _position(position) {}

        int getId(){return _id;}
        glm::vec3 getPosition(){return _position;}

        bool equals(glm::vec3 position)
        {
            if (position.x == _position.x && position.y == _position.y && position.z == _position.z)
                return true;

            return false;
        }

        std::string toString()
        {
            std::stringstream ss;
            ss << "{" << _position.x << "," << _position.y << "," << _position.z << "}";
            return std::string(ss.str());
        }

    private:
        int _id;
        glm::vec3 _position;
    };

    class GridPolygon
    {
    public:
        GridPolygon() {}
        GridPolygon(const int id, const glm::vec3 vertices[3]): _id(id)
        {
            for (int i = 0; i < 3; i++)
                _vertices[i] = vertices[i];
        }

        int getId(){return _id;}
        glm::vec3* getVertices() {return _vertices;}

    private:
        int _id;
        glm::vec3 _vertices[3];
    };

    typedef std::vector<Point> PointList;
    typedef std::vector<GridPolygon> PolygonList;

    struct GridCell
    {
    public:
        GridCell() {}
        GridCell(const uint x, const uint y, const uint z, const uint id = 0)
        {
            _position = glm::vec3(x, y, z);
            _id = id;
        }
        ~GridCell(){}

        glm::vec3 getPositions() {return _position;}
        PointList& getPoints() {return _points;}
        PolygonList& getPolygons() {return _polygons;}

        uint getId() {return _id;}
        bool isEmpty() {return _points.empty();}

        uint _id;
    private:

        glm::vec3 _position;
        PointList _points;
        PolygonList _polygons;
    };

    typedef std::vector<GridCell> Grid;

    class SpatialGrid
    {
    public:
        SpatialGrid();
        SpatialGrid(Volume domain);
        ~SpatialGrid();
        void initialize(Volume domain);

        void insertPoint(const int id, const glm::vec3 position);
        void insertPolygon(const int id, const glm::vec3 vertices[3]);
        GridCell& getCell(const uint cellIndex);

        uint getSize(){return _grid.size();}
        Volume getResolution(){return _resolution;}
        Volume getDomain(){return _domain;}

        uint getCellIndex(const glm::vec3 position);
        uint getCellIndex(const uint x, const uint y, const uint z);
        glm::vec3 getCellPosition(const uint cellIndex);
        int getPointIndex(const uint cellIndex, const glm::vec3 position);
        std::vector<uint> getNeighborCells(const uint cellIndex);
        int getCellId(const uint x, const uint y, const uint z);

    private:
        Volume _resolution;
        Volume _domain;
        Grid _grid;
    };

}

#endif // SPATIAL_GRID_H
