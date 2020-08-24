#include <iostream>

#include <synk/utils/logs/LogHandler.hpp>
#include <synk/utils/utilities/OS.hpp>
#include <synk/utils/time/Timer.hpp>

#include <synk/graphics/window/SynkWindow.hpp>
#include <synk/graphics/window/handlers/EventHandler.hpp>

#include <synk/graphics/OpenGL/Module.hpp>

using namespace synk;
using namespace synk::utils;
using namespace synk::utils::logs;
using namespace synk::utils::utilities;
using namespace synk::utils::time;
using namespace synk::graphics;
using namespace synk::graphics::window;
using namespace synk::graphics::OpenGL;

static inline auto  logs_   =   LogHandler("MAIN", Timer::getProgramTime);

int main(void)
{
    logs_.initlog();

        const auto  screens =   Screens::enumerateScreens();    // Enumerate all screens on the system

        logs_.slog("{} screen(s) found on this system", screens.size());

        auto    number  =   0u;
        for (const auto&    screen  :   screens)    // Enumerate properties of screens
        {
            number++;

            logs_.initlog("SCREEN {}", number);

                logs_.ilog("Position  : {{} ; {}}", screen.x, screen.y);
                logs_.ilog("Dimension : {{} ; {}}", screen.width, screen.height);

            logs_.exitlog("SCREEN {}", number);
        }

        const auto  video_settings  =   SynkWindow::VideoSettings // Set VideoSettings for our Window
        {
                .x      =   0,
                .y      =   0,
                .width  =   1280,
                .height =   720,
                .screen =   screens[0]
        };

        const auto  window_style    =   SynkWindow::WindowStyle::TitleBar |
                                        SynkWindow::WindowStyle::Close |
                                        SynkWindow::WindowStyle::Minimizable |
                                        SynkWindow::WindowStyle::Resizable;

        auto    window          =   SynkWindow::open("Synk Engine - Dev", video_settings, window_style); //  Create our window with a title, VideoSettings structure and the WindowStyle
        auto    opengl_module   =   window->extract<OpenGL::Module>();

        window->build();

        logs_.ilog("Window Title     : {}", window->getTitle());
        logs_.ilog("Window Position  : {} / {}", window->getPosX(), window->getPosY());
        logs_.ilog("Window Dimension : {} / {}", window->getWidth(), window->getHeight());

        opengl_module->setClearColor({ 248 / 255.0f, 142 / 255.0f, 85 / 255.0f, 0.0f });    //
        opengl_module->setVSync(true);                                                      // Parameters

        auto event_handler  =   window->getEventHandler();      // Get the EventHandler to manage viewport update when resized

        event_handler->add(SynkWindow::EventType::Resized, [&opengl_module](const SynkWindow::Event& event)
        {
            opengl_module->setViewport({ 0.0f, 0.0f, static_cast<float> (event.resized.width), static_cast<float> (event.resized.height) });    // Update each resized
        });

        // BEGIN - MESH PART

        const auto  vertices    =   std::vector<float>          // Make a vertices vector
        {   // Vertex position              Vertex color
            -0.5f, -0.5f, 0.0f,             139 / 255.0f,   217 / 255.0f,   231 / 255.0f,   1.0f,
            +0.0f, +0.75f, 0.0f,             4 / 255.0f,     60 / 255.0f,    90 / 255.0f,    1.0f,
            +0.5f, -0.5f, 0.0f,             139 / 255.0f,   217 / 255.0f,   231 / 255.0f,   1.0f,
        };

        const auto  indices     =   std::vector<unsigned int>   // Calculate indices
        {
            0, 1, 2
        };

        const auto  vao =   opengl_module->createArrayBuffer    ();                                                                         // Create a VAO
        const auto  vbo =   opengl_module->createVertexBuffer   (vertices.size() * sizeof(float),       GL::STATIC_DRAW, vertices.data());  // Create and fill a VBO
        const auto  ibo =   opengl_module->createIndexBuffer    (indices.size() * sizeof(unsigned int), GL::STATIC_DRAW, indices.data());   // Create and fill an IBO

        vao->bind();    // Bind it

            vbo->setAttribPointers({ 3, 4 }, GL::FLOAT); // Tell OpenGL how the data is

        vao->unbind();  // Unbind it

        // END - MESH PART

        const auto  main_shaders    =   opengl_module->loadShaders("res/shaders/main"); // Load shaders

        const auto  chrono  =   Timer::createChrono();
        chrono->start();

        while (window->isOpen())
        {
            Timer::update();

            window->pollEvents();   // detect and apply all events
            window->update();       // Update the window

            opengl_module->clearBuffers(GL::COLOR_BUFFER_BIT | GL::DEPTH_BUFFER_BIT);

            // Draw here

            main_shaders->bind();   // Bind the shader we want to use

                vao->drawElements(GL::TRIANGLES, indices.size(), GL::UNSIGNED_INT, *ibo); // Draw the data in the VAO with Indexing based on the IBO

            main_shaders->unbind(); // Unbind

            opengl_module->swapBuffers();   // change the buffer that will appear on the screen (Double-Buffering
        }

    logs_.exitlog();

    logs_.reset();
}
