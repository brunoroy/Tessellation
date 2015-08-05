#ifndef SCENEPLAYER_H
#define SCENEPLAYER_H

class ScenePlayer
{
public:
    ScenePlayer(int frameCount = -1):
        _play(false), _currentFrame(1), _frameCount(frameCount) {}
    ~ScenePlayer() {}

    void playPause() {_play = !_play;}
    bool isPaused() {return (_play==false);}

    int getNextFrame()
    {
        int nextFrame = _currentFrame++;
        if (nextFrame > _frameCount)
        {
            nextFrame = 1;
            _currentFrame = nextFrame;
        }

        return nextFrame;
    }

private:
    int _currentFrame;
    int _frameCount;
    bool _play;
};

#endif // SCENEPLAYER_H
