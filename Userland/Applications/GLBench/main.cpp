/*
 * Copyright (c) 2022, Stephan Unverwerth <s.unverwerth@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibCore/File.h>
#include <LibCore/System.h>
#include <LibGL/GL/gl.h>
#include <LibGL/GLContext.h>
#include <LibGUI/ActionGroup.h>
#include <LibGUI/Application.h>
#include <LibGUI/FilePicker.h>
#include <LibGUI/Icon.h>
#include <LibGUI/Label.h>
#include <LibGUI/Menu.h>
#include <LibGUI/Menubar.h>
#include <LibGUI/MessageBox.h>
#include <LibGUI/Painter.h>
#include <LibGUI/Widget.h>
#include <LibGUI/Window.h>
#include <LibGfx/Bitmap.h>
#include <LibGfx/Palette.h>
#include <LibMain/Main.h>
#include <LibThreading/Thread.h>
#include <AK/OwnPtr.h>
#include <LibCore/ArgsParser.h>

#include "Benchmark.h"

constexpr int RENDER_WIDTH = 800;
constexpr int RENDER_HEIGHT = 600;

RefPtr<Gfx::Bitmap> g_bitmap;

class GLContextWidget final : public GUI::Frame {
    C_OBJECT(GLContextWidget);

public:
    void render();

private:
    GLContextWidget()
    {
        start_timer(100);
    }

    virtual void paint_event(GUI::PaintEvent&) override;
    virtual void timer_event(Core::TimerEvent&) override;
};

void GLContextWidget::paint_event(GUI::PaintEvent& event)
{
    GUI::Frame::paint_event(event);

    GUI::Painter painter(*this);
    painter.add_clip_rect(event.rect());
    painter.draw_scaled_bitmap(frame_inner_rect(), *g_bitmap, g_bitmap->rect());
}

void GLContextWidget::timer_event(Core::TimerEvent&)
{
    update();
}

class FullscreenQuadScenario : public Scenario {
public:
    FullscreenQuadScenario()
        : Scenario("Fullscreen Quad"sv, "Render a fullscreen quad with varying texture sizes and wrap modes"sv)
    {
        add(m_texture_size);
        add(m_wrap_mode);

        m_texture_data.resize(2048 * 2048);
        for (int y = 0; y < 2048; ++y)
            for (int x = 0; x < 2048; ++x)
                m_texture_data[y * 2048 + x] = Color(x, y, x + y);
    }

    virtual void initialize_run() override
    {
        m_context = GL::create_context(*g_bitmap);
        GL::make_context_current(m_context);

        glGenTextures(1, &m_texture);
        glBindTexture(GL_TEXTURE_2D, m_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_texture_size.value(), m_texture_size.value(), 0, GL_RGBA, GL_UNSIGNED_BYTE, m_texture_data.data());

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_wrap_mode.value() );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_wrap_mode.value() );
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_LIGHTING);

        glClearColor(0.25f, 0.25f, 0.25f, 1.0f);

        // Set projection matrix
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, m_texture);
    }

    virtual void run_once(int) override
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBegin(GL_QUADS);
        
        glTexCoord2f(-1, -1);
        glVertex2f(-1, -1);
        
        glTexCoord2f(2, -1);
        glVertex2f(1, -1);
        
        glTexCoord2f(2, 2);
        glVertex2f(1, 1);
        
        glTexCoord2f(-1, 2);
        glVertex2f(-1, 1);
        
        glEnd();

        GL::present_context(m_context);
    }

private:
    GenericParameter<int> m_texture_size { "Texture Size", { 1, 16, 17, 512, 2048 } };
    GenericParameter<GLenum> m_wrap_mode { "Wrap Mode", {
        {GL_REPEAT, "GL_REPEAT"},
        {GL_MIRRORED_REPEAT, "GL_MIRRORED_REPEAT"},
        {GL_CLAMP_TO_EDGE, "GL_CLAMP_TO_EDGE"}
    }};
    GLuint m_texture {};
    OwnPtr<GL::GLContext> m_context;
    Vector<Gfx::Color> m_texture_data;
};


class ClearFramebufferSenario : public Scenario {
public:
    ClearFramebufferSenario()
        : Scenario("Clear Framebuffer"sv, "Clear the framebuffer with all combinations of clear mask"sv)
    {
        add(m_flags);
    }

    virtual void initialize_run() override
    {
        m_context = GL::create_context(*g_bitmap);
        GL::make_context_current(m_context);

        glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
        glClearDepth(1.0);
        glClearStencil(0);
    }

    virtual void run_once(int frame_number) override
    {
        glClearColor(frame_number * 0.0025f, frame_number * 0.005f, frame_number * 0.01f, 1.0f);
        glClear(m_flags.value());

        GL::present_context(m_context);
    }

private:
    GenericParameter<GLenum> m_flags { "Clear Flags", {
        {GL_COLOR_BUFFER_BIT, "GL_COLOR_BUFFER_BIT"},
        {GL_DEPTH_BUFFER_BIT, "GL_DEPTH_BUFFER_BIT"},
        {GL_STENCIL_BUFFER_BIT, "GL_STENCIL_BUFFER_BIT"},
        {GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, "GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT"},
        {GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT, "GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT"},
        {GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT, "GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT"},
        {GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT, "GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT"},
    }};
    OwnPtr<GL::GLContext> m_context;
};

class PresentScenario : public Scenario {
public:
    PresentScenario()
        : Scenario("Present Only"sv, "Only call present on the context and nothing else"sv)
    {
    }

    virtual void initialize_run() override
    {
        m_context = GL::create_context(*g_bitmap);
        GL::make_context_current(m_context);
    }

    virtual void run_once(int) override
    {
        GL::present_context(m_context);
    }

private:
    OwnPtr<GL::GLContext> m_context;
};

class TilesScenario : public Scenario {
public:
    TilesScenario()
        : Scenario("Tiles"sv, "Render a 512x512 quad with tiles of varying size"sv)
    {
        add(m_size);
    }

    virtual void initialize_run() override
    {
        m_context = GL::create_context(*g_bitmap);
        GL::make_context_current(m_context);

        glDisable(GL_TEXTURE_2D);
        glDisable(GL_DEPTH_TEST);

        glMatrixMode(GL_PROJECTION_MATRIX);
        glOrtho(0, RENDER_WIDTH, 0, RENDER_HEIGHT, -1, 1);
    }

    virtual void run_once(int) override
    {
        glBegin(GL_QUADS);
        int tile_size = m_size.value();
        for (int y = 0; y < 512; y += tile_size) {
            for (int x = 0; x < 512; x += tile_size) {
                glColor3ub(x / 2, y / 2, 0);
                glVertex2i(x, y);
                glVertex2i(x + tile_size, y);
                glVertex2i(x + tile_size, y + tile_size);
                glVertex2i(x, y + tile_size);
            }
        }
        glEnd();

        GL::present_context(m_context);
    }

private:
    GenericParameter<int> m_size { "Tile Size", {4, 8, 16, 32, 64, 128}};
    OwnPtr<GL::GLContext> m_context;
};

ErrorOr<int> serenity_main(Main::Arguments arguments)
{
    auto app = GUI::Application::construct(arguments);

    TRY(Core::System::pledge("stdio thread recvfd sendfd rpath unix prot_exec"));

    TRY(Core::System::unveil("/res", "r"));
    TRY(Core::System::unveil("/usr/lib", "r"));
    TRY(Core::System::unveil(nullptr, nullptr));
    
    bool nogui = false;
    bool list_scenarios = false;
    String selected_scenario;

    Core::ArgsParser args_parser;
    args_parser.set_general_help("Measure LibGL and GPU performance.");
    args_parser.add_option(list_scenarios, "List scenarios", "list", 'l');
    args_parser.add_option(nogui, "Do not show rendering output", "nogui", 'n');
    args_parser.add_option(selected_scenario, "Run only this scenario", "scenario", 's', "Scenario");
    args_parser.parse(arguments);

    g_bitmap = Gfx::Bitmap::try_create(Gfx::BitmapFormat::BGRx8888, { RENDER_WIDTH, RENDER_HEIGHT }).release_value_but_fixme_should_propagate_errors();

    Benchmark benchmark;
    benchmark.add(adopt_own(*new ClearFramebufferSenario()));
    benchmark.add(adopt_own(*new PresentScenario()));
    benchmark.add(adopt_own(*new FullscreenQuadScenario()));
    benchmark.add(adopt_own(*new TilesScenario()));

    if (list_scenarios) {
        for (auto& scenario : benchmark.scenarios()) {
            outln("'{}' {}", scenario->name(), scenario->description());
        }
        return 0;
    }

    auto render_thread = MUST(Threading::Thread::try_create([&] {
        benchmark.run(selected_scenario);
        app->quit();
        return 0;
    }));

    render_thread->start();

    int app_result = 0;

    if (!nogui) {
        // Construct the main window
        auto window = GUI::Window::construct();
        auto app_icon = GUI::Icon::default_icon("app-3d-file-viewer");
        window->set_icon(app_icon.bitmap_for_size(16));
        window->set_title("LibGL Benchmark");
        window->resize(RENDER_WIDTH + 4, RENDER_HEIGHT + 4);
        window->set_resizable(false);
        window->set_double_buffering_enabled(true);
        auto widget = TRY(window->try_set_main_widget<GLContextWidget>());

        window->show();
    
        app_result = app->exec();
    }

    // Wait for benchmark to end
    auto joined_or_error = render_thread->join();

    return app_result;
}
