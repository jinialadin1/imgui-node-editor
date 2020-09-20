# include "application.h"
# include "setup.h"
# include "platform.h"
# include "renderer.h"

extern "C" {
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#include "stb_image.h"
}


//# if BACKEND(IMGUI_WIN32)
//#     include "dx11/imgui_impl_win32.h"
//# endif
//
//# if BACKEND(IMGUI_GLFW)
//#     include <GLFW/glfw3.h>
//
//#     include "imgui_impl_glfw.h"
//# endif
//
//# if RENDERER(IMGUI_DX11)
//#     include "dx11/imgui_impl_dx11.h"
//# endif
//
//# if RENDERER(IMGUI_OGL3)
//#     include "imgui_impl_opengl3.h"
//# endif


Application::Application(const char* name)
    : Application(name, 0, nullptr)
{
}

Application::Application(const char* name, int argc, char** argv)
    : m_Name(name)
    , m_Platform(CreatePlatform(*this))
    , m_Renderer(CreateRenderer())
{
    m_Platform->ApplicationStart(argc, argv);
}

Application::~Application()
{
    m_Renderer->Destroy();

    m_Platform->ApplicationStop();

    if (m_Context)
    {
        ImGui::DestroyContext(m_Context);
        m_Context= nullptr;
    }
}

bool Application::Create(int width /*= -1*/, int height /*= -1*/)
{
    m_Context = ImGui::CreateContext();
    ImGui::SetCurrentContext(m_Context);

    if (!m_Platform->OpenMainWindow("Application", width, height))
        return false;

    if (!m_Renderer->Create(*m_Platform))
        return false;

    std::string iniFilename = m_Name + ".ini";

    ImGuiIO& io = ImGui::GetIO();
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    io.IniFilename = iniFilename.c_str();
    io.LogFilename = nullptr;

    ImFontConfig config;
    config.OversampleH = 4;
    config.OversampleV = 4;
    config.PixelSnapH = false;

    m_DefaultFont = io.Fonts->AddFontFromFileTTF("data/Play-Regular.ttf", 18.0f, &config);
    m_HeaderFont  = io.Fonts->AddFontFromFileTTF("data/Cuprum-Bold.ttf", 20.0f, &config);
    io.Fonts->Build();

    //// Setup ImGui binding
    //ImGui_ImplWin32_Init(hwnd);
    //ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);
    //AX_SCOPE_EXIT
    //{
    //    ImGui_ImplDX11_Shutdown();
    //    ImGui_ImplWin32_Shutdown();
    //};

    ImGui::StyleColorsDark();

    //ImGuiStyle& style = ImGui::GetStyle();
    //style.FrameRounding     = 2.0f;
    //style.WindowRounding    = 0.0f;
    //style.ScrollbarRounding = 0.0f;

    //ImVec4 backgroundColor = ImColor(32, 32, 32, 255);//style.Colors[ImGuiCol_TitleBg];

    OnStart();

    Frame();

    return true;
}

int Application::Run()
{
    m_Platform->ShowMainWindow();

    while (m_Platform->ProcessMainWindowEvents())
    {
        if (!m_Platform->IsMainWindowVisible())
            continue;

        Frame();
    }

    OnStop();

    return 0;
}

void Application::Frame()
{
    auto& io = ImGui::GetIO();

    m_Platform->NewFrame();
    m_Renderer->NewFrame();

    ImGui::NewFrame();

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(io.DisplaySize);
    ImGui::Begin("Content", nullptr, GetWindowFlags());

    OnFrame(io.DeltaTime);

    ImGui::End();

    // Rendering
    m_Renderer->Clear(ImColor(32, 32, 32, 255));
    ImGui::Render();
    m_Renderer->RenderDrawData(ImGui::GetDrawData());

    m_Platform->FinishFrame();
}

void Application::SetTitle(const char* title)
{
    m_Platform->SetMainWindowTitle(title);
}

bool Application::Close()
{
    return m_Platform->CloseMainWindow();
}

void Application::Quit()
{
    m_Platform->Quit();
}

const std::string& Application::GetName() const
{
    return m_Name;
}

ImFont* Application::DefaultFont() const
{
    return m_DefaultFont;
}

ImFont* Application::HeaderFont() const
{
    return m_HeaderFont;
}

ImTextureID Application::LoadTexture(const char* path)
{
    int width = 0, height = 0, component = 0;
    if (auto data = stbi_load(path, &width, &height, &component, 4))
    {
        auto texture = CreateTexture(data, width, height);
        stbi_image_free(data);
        return texture;
    }
    else
        return nullptr;
}

ImTextureID Application::CreateTexture(const void* data, int width, int height)
{
    return m_Renderer->CreateTexture(data, width, height);
}

void Application::DestroyTexture(ImTextureID texture)
{
    m_Renderer->DestroyTexture(texture);
}

int Application::GetTextureWidth(ImTextureID texture)
{
    return m_Renderer->GetTextureWidth(texture);
}

int Application::GetTextureHeight(ImTextureID texture)
{
    return m_Renderer->GetTextureHeight(texture);
}

ImGuiWindowFlags Application::GetWindowFlags() const
{
    return
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoScrollWithMouse |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoBringToFrontOnFocus;
}
