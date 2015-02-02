#pragma once



struct StartupMesurements {
    float sdlStartup = 0.f,
          graphicsStartup = 0.f,
          debugStartup = 0.f,
          resourceStartup = 0.f,
          sceneStartup = 0.f,
          totalTime = 0.f;
};