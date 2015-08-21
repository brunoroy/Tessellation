#include "spatialGrid.h"
#include <iostream>

namespace Tessellation
{

    SpatialGrid::SpatialGrid()
    {

    }

    SpatialGrid::SpatialGrid(Volume domain)
    {
        initialize(domain);
    }

    SpatialGrid::~SpatialGrid()
    {
        _grid.clear();
    }

    void SpatialGrid::initialize(Volume domain)
    {
        _domain = domain;
        float width = pow(2.0, ceil(_domain.getWidth())+2);
        float height = pow(2.0, ceil(_domain.getHeight())+2);
        float depth = pow(2.0, ceil(_domain.getDepth())+2);
        _resolution = Volume(width, height, depth);

        _grid.clear();
        for (uint y = 0; y < static_cast<uint>(_resolution.getHeight()); ++y)
        {
            for (uint z = 0; z < static_cast<uint>(_resolution.getDepth()); ++z)
            {
                for (uint x = 0; x < static_cast<uint>(_resolution.getWidth()); ++x)
                {
                    uint id = (y * (_resolution.getWidth()*_resolution.getDepth()) + z * _resolution.getWidth() + x);
                    _grid.push_back(GridCell(x, y, z, id));
                }
            }
        }
    }

    void SpatialGrid::insertPoint(const int id, const glm::vec3 position)
    {
        uint cellIndex = getCellIndex(position);
        Point point(id, position);
        _grid[cellIndex].getPoints().push_back(point);
    }

    void SpatialGrid::insertPolygon(const int id, const glm::vec3 vertices[3])
    {
        glm::vec3 meanPosition(0.0f);
        for (int i = 0; i < 3; i++)
            meanPosition += vertices[i];
        meanPosition /= 3;
        //std::clog << "mean: {" << meanPosition.x << "," << meanPosition.y << "," << meanPosition.z << "}" << std::endl;

        uint cellIndex = getCellIndex(meanPosition);
        //std::clog << "cell: " << cellIndex << std::endl;
        GridPolygon polygon(id, vertices);
        _grid[cellIndex].getPolygons().push_back(polygon);
    }

    GridCell& SpatialGrid::getCell(const uint cellIndex)
    {
        return _grid.at(cellIndex);
    }

    uint SpatialGrid::getCellIndex(const glm::vec3 position)
    {
        Volume cellVolume;
        cellVolume.setWidth(_domain.getWidth()/_resolution.getWidth());
        cellVolume.setHeight(_domain.getHeight()/_resolution.getHeight());
        cellVolume.setDepth(_domain.getDepth()/_resolution.getDepth());

        uint gridWidth = static_cast<uint>(_resolution.getWidth());
        uint gridHeight = static_cast<uint>(_resolution.getHeight());
        uint gridDepth = static_cast<uint>(_resolution.getDepth());

        uint xCellIndex = static_cast<uint>(position.x/cellVolume.getWidth());
        uint yCellIndex = static_cast<uint>(position.y/cellVolume.getHeight());
        uint zCellIndex = static_cast<uint>(position.z/cellVolume.getDepth());

        if (xCellIndex == gridWidth)
            xCellIndex = gridWidth-1;
        if (yCellIndex == gridHeight)
            yCellIndex = gridHeight-1;
        if (zCellIndex == gridDepth)
            zCellIndex = gridDepth-1;

        uint cellIndex = xCellIndex + yCellIndex * (gridWidth*gridDepth) + (zCellIndex*gridDepth);

        return cellIndex;
    }

    uint SpatialGrid::getCellIndex(const uint x, const uint y, const uint z)
    {
        uint gridWidth = static_cast<uint>(_resolution.getWidth());
        uint gridDepth = static_cast<uint>(_resolution.getDepth());

        return x + y * (gridWidth*gridDepth) + (z*gridDepth);
    }

    glm::vec3 SpatialGrid::getCellPosition(const uint cellIndex)
    {
        uint dimXZ = static_cast<uint>(_resolution.getWidth()*_resolution.getDepth());

        uint cellY = static_cast<uint>(std::floor((float)cellIndex / (_resolution.getWidth()*_resolution.getDepth())));
        uint cellX = (cellIndex-(cellY*dimXZ)) % static_cast<uint>(_resolution.getWidth());
        uint cellZ = static_cast<uint>(std::floor(static_cast<float>(cellIndex-(cellY*dimXZ)) / _resolution.getWidth()));

        glm::vec3 cellPosition(static_cast<float>(cellX), static_cast<float>(cellY), static_cast<float>(cellZ));

        return cellPosition;
    }

    int SpatialGrid::getPointIndex(const uint cellIndex, const glm::vec3 position)
    {
        for (int i = 0; i < _grid.at(cellIndex).getPoints().size(); ++i)
        {
            Point& point = _grid.at(cellIndex).getPoints().at(i);
            if (point.equals(position))
                return i;
        }

        return -1;
    }

    int SpatialGrid::getCellId(const uint x, const uint y, const uint z)
    {
        if (x < 0 || x > (static_cast<uint>(_resolution.getWidth())-1))
            return -1;
        if (y < 0 || y > (static_cast<uint>(_resolution.getHeight())-1))
            return -1;
        if (z < 0 || z > (static_cast<uint>(_resolution.getDepth())-1))
            return -1;

        int getCellId = (y * (_resolution.getWidth()*_resolution.getDepth()) + z * _resolution.getWidth() + x);

        return getCellId;
    }

    std::vector<uint> SpatialGrid::getNeighborCells(const uint cellIndex)
    {
        std::vector<uint> neighborCells;
        GridCell& cell = getCell(cellIndex);
        glm::vec3 c = cell.getPositions();

        for (int y = -1; y <= 1; ++y)
        {
            for (int z = -1; z <= 1; ++z)
            {
                for (int x = -1; x <= 1; ++x)
                {
                    int cellId = getCellId(c.x+x, c.y+y, c.z+z);
                    if (cellId != -1)
                    {
                        GridCell& neighborCell = getCell(cellId);
                        if (!neighborCell.isEmpty())
                            neighborCells.push_back(cellId);
                    }
                }
            }
        }

        return neighborCells;
    }

}
