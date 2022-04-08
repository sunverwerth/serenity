/*
 * Copyright (c) 2022, Stephan Unverwerth <s.unverwerth@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibCore/ElapsedTimer.h>
#include <LibCore/File.h>
#include <LibCore/System.h>
#include <LibFileSystemAccessClient/Client.h>
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
        m_bitmap = Gfx::Bitmap::try_create(Gfx::BitmapFormat::BGRx8888, { RENDER_WIDTH, RENDER_HEIGHT }).release_value_but_fixme_should_propagate_errors();
        m_context = GL::create_context(*m_bitmap);

        start_timer(0);

        GL::make_context_current(m_context);
    }

    virtual void paint_event(GUI::PaintEvent&) override;
    virtual void timer_event(Core::TimerEvent&) override;

private:
    RefPtr<Gfx::Bitmap> m_bitmap;
    OwnPtr<GL::GLContext> m_context;
};

void GLContextWidget::paint_event(GUI::PaintEvent& event)
{
    GUI::Frame::paint_event(event);

    GUI::Painter painter(*this);
    painter.add_clip_rect(event.rect());
    //painter.draw_scaled_bitmap(frame_inner_rect(), *m_bitmap, m_bitmap->rect());
    painter.draw_scaled_bitmap(frame_inner_rect(), *g_bitmap, g_bitmap->rect());
}

void GLContextWidget::timer_event(Core::TimerEvent&)
{
    //render();
    update();
}

void GLContextWidget::render()
{
    //auto timer = Core::ElapsedTimer::start_new();

    GL::make_context_current(m_context);

    glFrontFace(GL_CCW);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    // Enable lighting
    glDisable(GL_LIGHTING);

    glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
    glClearDepth(1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glBegin(GL_QUADS);
    glColor3f(1, 0, 0);
    glVertex2f(-1, -1);
    glVertex2f(1, -1);
    glVertex2f(1, 1);
    glVertex2f(-1, 1);
    glEnd();

    m_context->present();
}

class LibGLBenchmark : public Benchmark {
public:
    virtual void initialize() override
    {
        g_bitmap = Gfx::Bitmap::try_create(Gfx::BitmapFormat::BGRx8888, { RENDER_WIDTH, RENDER_HEIGHT }).release_value_but_fixme_should_propagate_errors();
        m_context = GL::create_context(*g_bitmap);
        GL::make_context_current(m_context);
    }

    virtual void shutdown() override
    {
        GL::present_context(m_context);
        m_context = nullptr;
    }

private:
    OwnPtr<GL::GLContext> m_context;
};

class FullscreenQuadScenario : public Scenario {
public:
    FullscreenQuadScenario()
        : Scenario("Fullscreen Quad"sv)
    {
        add(m_texture_size);
        add(m_texture_format);
        add(m_wrap_mode);
        add(m_enable_textures);
    }

    virtual void initialize_run() override
    {
        glGenTextures(1, &m_texture);
        glBindTexture(GL_TEXTURE_2D, m_texture);
        unsigned char* data = new unsigned char[4 * m_texture_size.value() * m_texture_size.value()];
        for (int i = 0; i < m_texture_size.value() * m_texture_size.value() * 4; i++)
            data[i] = rand();
        glTexImage2D(GL_TEXTURE_2D, 0, m_texture_format.value(), m_texture_size.value(), m_texture_size.value(), 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        delete[] data;

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_wrap_mode.value() );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_wrap_mode.value() );
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_LIGHTING);

        glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
        glClearDepth(1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Set projection matrix
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();


        if (m_enable_textures.value())
            glEnable(GL_TEXTURE_2D);
        else
            glDisable(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, m_texture);
    }

    virtual void shutdown_run() override
    {
        glBindTexture(GL_TEXTURE_2D, 0);
        glDeleteTextures(1, &m_texture);
    }

    virtual void run_once() override
    {
        glBegin(GL_QUADS);
        
        glTexCoord2f(0, 0);
        glVertex2f(-1, -1);
        
        glTexCoord2f(1, 0);
        glVertex2f(1, -1);
        
        glTexCoord2f(1, 1);
        glVertex2f(1, 1);
        
        glTexCoord2f(0, 1);
        glVertex2f(-1, 1);
        
        glEnd();
    }

private:
    GenericParameter<int> m_texture_size { "Texture Size", { 16, 2048 } };
    GenericParameter<int> m_texture_format { "Texture Format", { 4 } };
    GenericParameter<GLenum> m_wrap_mode { "Wrap Mode", { GL_REPEAT, GL_MIRRORED_REPEAT, GL_CLAMP_TO_EDGE } };
    GenericParameter<bool> m_enable_textures { "Texturing Enabled", { true, false } };
    GLuint m_texture {};
};

ErrorOr<int> serenity_main(Main::Arguments arguments)
{
    auto app = GUI::Application::construct(arguments);

    TRY(Core::System::pledge("stdio thread recvfd sendfd rpath unix prot_exec"));

    TRY(Core::System::unveil("/tmp/portal/filesystemaccess", "rw"));
    TRY(Core::System::unveil("/home/anon/Documents/3D Models", "r"));
    TRY(Core::System::unveil("/res", "r"));
    TRY(Core::System::unveil("/usr/lib", "r"));
    TRY(Core::System::unveil(nullptr, nullptr));

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

    LibGLBenchmark benchmark;
    benchmark.add(adopt_own(*new FullscreenQuadScenario()));
    benchmark.run();

    return app->exec();
}
