#pragma once

#include <GameEngine.h>
#include <SDL_rect.h>

union SDL_Event;
struct SDL_Texture;

class b2World;
class b2Body;
class b2Fixture;

class Game: public GameEngine
{
  friend class GameEngine;

public:
  ~Game();

protected:
  Game();

  void InitializeImpl();
  void UpdateImpl(float dt);
  void DrawImpl(SDL_Renderer *renderer, float dt);

  void Reset();
  void CalculateDrawOrder(std::vector<GameObject *>& drawOrder);

  void DrawShape(SDL_Renderer *renderer, b2Body *body, b2Fixture *fixture);

  /* Test texture properties. */
  SDL_Texture *_testTexture;
  SDL_Rect _testTextureBounds;
  float _testTextureAngleDegrees;

  SDL_Point _testTextureRotationCenter;
  Vector2 _testTexturePosition;

  b2World *_world;
  b2Body *_boxBody;
  b2Fixture *_boxFixture;
};