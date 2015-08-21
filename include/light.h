#ifndef LIGHT_H
#define LIGHT_H

#include <GL/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Tessellation
{

    class Light
    {
    public:
        Light(glm::vec3 position): _position(position), _mvp(glm::mat4(1.0f)) {}
        ~Light() {}

        glm::vec3 getPosition() {return _position;}
        glm::mat4 getMVP() {return _mvp;}
        void setMVP(glm::mat4 mvp)
        {
            _mvp = mvp * _modelMatrix;
        }

        void setPosition(glm::vec3 position) {_position = position;}

        glm::mat4 getModelMatrix() {return _modelMatrix;}

    protected:
        glm::vec3 _position;
        glm::mat4 _modelMatrix;
        glm::mat4 _viewMatrix;
        glm::mat4 _mvp;
    };

    class DirectionalLight : public Light
    {
    public:
        DirectionalLight(glm::vec3 position, glm::vec3 direction):
            Light(position), _direction(direction), _distance(2.0) {}

        ~DirectionalLight() {}

        glm::vec3 getDirection() {return _direction;}
        void setDirection(glm::vec3 direction) {_direction = direction;}

        glm::vec3 getScreenSpacePosition()
        {
            glm::vec3 normPosition;
            if (_mvp[3][3] > std::numeric_limits<float>::epsilon())
                normPosition = glm::vec3(_mvp[3][0], _mvp[3][1], _mvp[3][2]) / _mvp[3][3];
            else
                normPosition = glm::vec3(0.0f, 0.0f, 0.0f);

            _screenSpacePosition = 0.5f * normPosition + glm::vec3(0.5f, 0.5f, 0.5f);
            //std::clog << "ss: {" << _screenSpacePosition.x << "," << _screenSpacePosition.y << "," << _screenSpacePosition.z << "}" << std::endl;

            return _screenSpacePosition;
        }

        void update(glm::vec3 camera, glm::vec3 constraints)
        {
            _constraints = constraints;

            _localSpacePosition = glm::vec3(cos(M_PI/3.0), sin(M_PI/3.0), -sin(M_PI/3.0)/3.0);
            glm::vec3 position = camera + _distance * _localSpacePosition;

            glm::vec3 z = glm::normalize(glm::vec3(camera - position));
            glm::vec3 x = glm::normalize(glm::cross(z, glm::vec3(0.0f, 1.0f, 0.0f)));
            glm::vec3 y = glm::normalize(glm::cross(x, z));

            _modelMatrix = glm::mat4(glm::vec4(x, 0.0f),
                             glm::vec4(y, 0.0f),
                             glm::vec4(-z, 0.0f),
                             glm::vec4(position, 1.0f));
        }

    private:
        float _distance;
        glm::vec3 _direction;
        glm::vec3 _screenSpacePosition;
        glm::vec3 _localSpacePosition;
        glm::vec3 _constraints;
    };

}

#endif // LIGHT_H
