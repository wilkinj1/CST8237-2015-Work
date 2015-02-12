#include "Game.h"
#include <GameObject.h>
#include <SDL.h>
#include <math.h>
#include <SDL_image.h>

#include <Box2D\Box2D.h>

// Initializing our static member pointer.
GameEngine* GameEngine::_instance = nullptr;

GameEngine* GameEngine::CreateInstance()
{
  if (_instance == nullptr)
  {
    _instance = new Game();
  }
  return _instance;
}

Game::Game() : GameEngine()
{

}

Game::~Game()
{
}

void Game::InitializeImpl()
{
  SDL_SetWindowTitle(_window, "Game");

  for (auto itr = _objects.begin(); itr != _objects.end(); itr++)
  {
    (*itr)->Initialize(_renderer);
  }

  // Load our texture.
  _testTexture = IMG_LoadTexture(_renderer, "res/UV.jpg");
  _testTextureAngleDegrees = 0.0f;

  /* Query the texture to get it's size. We then use it's size to offset where the texture will draw relative to,
   * since the default is it's top corner (0,0). */
  _testTextureBounds = { 0, 0, 0, 0 };
  SDL_QueryTexture(_testTexture, nullptr, nullptr, &_testTextureBounds.w, &_testTextureBounds.h);
  _testTextureBounds.x -= _testTextureBounds.w / 2;
  _testTextureBounds.y -= _testTextureBounds.h / 2;

  /* This center will only be the point we rotate around; not the point we draw relative to. So if I set this center to be the middle
  * of the texture, it will still draw at (0,0), and rotate around (250,250), which is where the middle of the texture would be. */
  _testTextureRotationCenter = { _testTextureBounds.w / 2, _testTextureBounds.h / 2 };

  // Get the window size so we can draw it in the center of the screen later.
  SDL_Point windowSize;
  SDL_GetWindowSize(_window, &windowSize.x, &windowSize.y);

  _testTexturePosition = { windowSize.x / 2, windowSize.y / 2 };


  // Initialize our Box2D world.
  b2Vec2 gravity(0.0f, 1.0f);

  _world = new b2World(gravity);

  // Create the definition for our body.
  b2BodyDef boxBodyDef;
  boxBodyDef.type = b2_dynamicBody;

  _boxBody = _world->CreateBody(&boxBodyDef);
  b2Vec2 boxPosition(250, 0);
  _boxBody->SetTransform(boxPosition, 0.0f);

  b2PolygonShape boxShape;
  boxShape.SetAsBox(5, 5);

  _boxFixture = _boxBody->CreateFixture(&boxShape, 0.0f);

}

void Game::UpdateImpl(float dt)
{
  SDL_Event evt;
  SDL_PollEvent(&evt);

  for (auto itr = _objects.begin(); itr != _objects.end(); itr++)
  {
    (*itr)->Update(dt);
  }

  // Increase the test texture's angle to get a continuous rotation.
  _testTextureAngleDegrees += (10.0f * dt);
}

void Game::DrawImpl(SDL_Renderer *renderer, float dt)
{
  std::vector<GameObject *> renderOrder;
  CalculateDrawOrder(renderOrder);
  
  /* Take the boundaries (-250,-250, 500, 500), and add the location of where we want to draw the texture to them so that it will be
   * properly centered around that point. */
  SDL_Rect drawRect = _testTextureBounds;
  drawRect.x += _testTexturePosition.x;
  drawRect.y += _testTexturePosition.y;

  SDL_RenderCopyEx(renderer, _testTexture, nullptr, &drawRect, _testTextureAngleDegrees, &_testTextureRotationCenter, SDL_FLIP_NONE);

  for (auto itr = renderOrder.begin(); itr != renderOrder.end(); itr++)
  {
    (*itr)->Draw(renderer, dt);
  }
}

void Game::CalculateDrawOrder(std::vector<GameObject *>& drawOrder)
{
  // SUPER HACK GARBAGE ALGO.
  drawOrder.clear();

  auto objectsCopy = _objects;
  auto farthestEntry = objectsCopy.begin();
  while (objectsCopy.size() > 0)
  {
    bool entryFound = true;
    for (auto itr = objectsCopy.begin(); itr != objectsCopy.end(); itr++)
    {
      if (farthestEntry != itr)
      {
        if ((*itr)->GetTransform().position.y < (*farthestEntry)->GetTransform().position.y)
        {
          entryFound = false;
          farthestEntry = itr;
          break;
        }
      }
    }

    if (entryFound)
    {
      GameObject *farthest = *farthestEntry;

      drawOrder.push_back(farthest);
      objectsCopy.erase(farthestEntry);
      farthestEntry = objectsCopy.begin();
    }
  }
}

void Game::DrawShape(SDL_Renderer *renderer, b2Body *body, b2Fixture *fixture)
{
  SDL_Point points[4];
  b2Vec2 position = body->GetPosition();

  b2PolygonShape* polygonShape = (b2PolygonShape *)fixture->GetShape();
  for (int vertexIndex = 0; vertexIndex < polygonShape->GetVertexCount(); vertexIndex++)
  {
    const b2Vec2 vertex = polygonShape->GetVertex(vertexIndex);
    points[vertexIndex].x = (float)(vertex.x + position.x);
    points[vertexIndex].y = (float)(vertex.y + position.y);
  }

  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);

  for (int vertexIndex = 0; vertexIndex < polygonShape->GetVertexCount(); vertexIndex++)
  {
    SDL_Point &firstPoint = points[vertexIndex];
    SDL_Point &secondPoint = points[(vertexIndex + 1) % polygonShape->GetVertexCount()];

    SDL_RenderDrawLine(renderer, firstPoint.x, firstPoint.y, secondPoint.x, secondPoint.y);
  }
}
