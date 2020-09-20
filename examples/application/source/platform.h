# pragma once
# include "setup.h"
# include <memory>

struct Application;
struct Renderer;

struct Platform
{
    virtual ~Platform() {};

    virtual bool ApplicationStart(int argc, char** argv) = 0;
    virtual void ApplicationStop() = 0;

    virtual bool OpenMainWindow(const char* title, int width, int height) = 0;
    virtual bool CloseMainWindow() = 0;
    virtual void* GetMainWindowHandle() const = 0;
    virtual void SetMainWindowTitle(const char* title) = 0;
    virtual void ShowMainWindow() = 0;
    virtual bool ProcessMainWindowEvents() = 0;
    virtual bool IsMainWindowVisible() const = 0;

    virtual void SetRenderer(Renderer* renderer) = 0;

    virtual void NewFrame() = 0;
    virtual void FinishFrame() = 0;

    virtual void Quit() = 0;

};

std::unique_ptr<Platform> CreatePlatform(Application& application);
