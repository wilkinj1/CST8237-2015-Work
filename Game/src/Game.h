#pragma once

#include <GameEngine.h>
#include <SDL_rect.h>

union SDL_Event;
struct SDL_Texture;

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

  /* Test texture properties. */
  SDL_Texture *_testTexture;
  SDL_Rect _testTextureBounds;
  float _testTextureAngleDegrees;

  SDL_Point _testTextureRotationCenter;
  Vector2 _testTexturePosition;
};