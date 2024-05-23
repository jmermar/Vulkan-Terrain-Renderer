#include "engine.hpp"

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <imgui_impl_sdl3.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/matrix.hpp>

#include "types.hpp"
namespace engine {

class Window : public val::PresentationProvider {
   private:
    SDL_Window* window{};

   public:
    Window(Size size, const char* name) {
        SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
        window = SDL_CreateWindow(name, size.w, size.h, SDL_WINDOW_VULKAN);
    }
    ~Window() { SDL_DestroyWindow(window); }

    operator SDL_Window*() { return window; }

    VkSurfaceKHR getSurface(VkInstance instance) override {
        VkSurfaceKHR sur;
        SDL_Vulkan_CreateSurface(window, instance, nullptr, &sur);
        return sur;
    }

    Size getSize() override {
        int w, h;
        SDL_GetWindowSize(window, &w, &h);
        return {(uint32_t)w, (uint32_t)h};
    }

    void initImgui() override { ImGui_ImplSDL3_InitForVulkan(window); }
};

struct SkyboxPushConstants {
    glm::mat4 transform;
    val::BindPoint<val::StorageBuffer> globalDataBind;
    val::BindPoint<val::Texture> skyboxTexture;
};

struct SkyboxVertexData {
    glm::vec3 pos;
    glm::vec3 coords;
};

void buildFace(std::span<SkyboxVertexData> vertices,
               std::span<uint32_t> indices, size_t firstVertex,
               size_t firstIndex, glm::vec3 topLeft, glm::vec3 right,
               glm::vec3 down, float tex) {
    vertices[firstVertex + 0].pos = topLeft;
    vertices[firstVertex + 1].pos = topLeft + right;
    vertices[firstVertex + 2].pos = topLeft + right + down;
    vertices[firstVertex + 3].pos = topLeft + down;

    vertices[firstVertex + 0].coords = {0, 0, tex};
    vertices[firstVertex + 1].coords = {1, 0, tex};
    vertices[firstVertex + 2].coords = {1, 1, tex};
    vertices[firstVertex + 3].coords = {0, 1, tex};

    indices[firstIndex + 0] = firstVertex;
    indices[firstIndex + 1] = firstVertex + 1;
    indices[firstIndex + 2] = firstVertex + 2;
    indices[firstIndex + 3] = firstVertex;
    indices[firstIndex + 4] = firstVertex + 2;
    indices[firstIndex + 5] = firstVertex + 3;
}

class SkyboxRenderer {
    friend class Engine;

   private:
    val::Engine& engine;
    val::BufferWriter& writer;
    val::Texture* skybox;
    val::Mesh* mesh;
    val::GraphicsPipeline pipeline{};

   public:
    SkyboxRenderer(val::Engine& engine, val::BufferWriter& writer)
        : engine(engine), writer(writer) {
        std::string textures[6] = {
            "textures/skybox/right.bmp", "textures/skybox/left.bmp",
            "textures/skybox/up.bmp",    "textures/skybox/down.bmp",
            "textures/skybox/back.bmp",  "textures/skybox/front.bmp"};
        ImageData skyboxImage = file::loadImageArray(std::span(textures));

        auto vertShader = file::readBinary("shaders/skybox.vert.spv");
        auto fragShader = file::readBinary("shaders/skybox.frag.spv");

        val::PipelineBuilder builder(engine);
        pipeline =
            builder.setPushConstant<SkyboxPushConstants>()
                .addVertexInputAttribute(0, val::VertexInputFormat::FLOAT3)
                .addVertexInputAttribute(offsetof(SkyboxVertexData, coords),
                                         val::VertexInputFormat::FLOAT3)
                .addColorAttachment(val::TextureFormat::RGBA16)
                .disableDepthTest()
                .addStage(std::span(vertShader), val::ShaderStage::VERTEX)
                .addStage(std::span(fragShader), val::ShaderStage::FRAGMENT)
                .fillTriangles()
                .setVertexStride(sizeof(SkyboxVertexData))
                .build();

        skybox =
            engine.createTexture(skyboxImage.size, val::TextureFormat::RGBA8,
                                 val::TextureSampler::LINEAR);
        writer.enqueueTextureWrite(skybox, skyboxImage.data.data());

        std::vector<SkyboxVertexData> vertices(6 * 4);
        std::vector<uint32_t> indices(6 * 6);

        // Top
        buildFace(vertices, indices, 0, 0, {-1, 1, 1}, {2, 0, 0}, {0, 0, -2},
                  0);
        // Bottom
        buildFace(vertices, indices, 4, 6, {-1, -1, -1}, {2, 0, 0}, {0, 0, 2},
                  1);
        // Front
        buildFace(vertices, indices, 8, 12, {1, 1, 1}, {-2, 0, 0}, {0, -2, 0},
                  2);
        // Back
        buildFace(vertices, indices, 12, 18, {-1, 1, -1}, {2, 0, 0}, {0, -2, 0},
                  3);
        // Left
        buildFace(vertices, indices, 16, 24, {-1, 1, 1}, {0, 0, -2}, {0, -2, 0},
                  4);
        // Right
        buildFace(vertices, indices, 20, 30, {1, 1, -1}, {0, 0, 2}, {0, -2, 0},
                  5);

        mesh = engine.createMesh(vertices.size() * sizeof(SkyboxVertexData),
                                 indices.size());

        writer.enqueueMeshWrite(mesh, std::span(vertices), std::span(indices));
    }

    void renderPass(val::Texture* framebuffer, const RenderState& rs,
                    val::CommandBuffer& cmd) {
        auto cmdb = cmd.cmd;

        cmd.beginPass(std::span(&framebuffer, 1));
        SkyboxPushConstants pc;
        pc.globalDataBind = rs.globalData;
        pc.skyboxTexture = skybox->bindPoint;
        pc.transform = glm::translate(glm::mat4(1), rs.cam.pos) *
                       glm::scale(glm::mat4(1), glm::vec3(500.f));
        cmd.bindPipeline(pipeline);
        cmd.pushConstants(pipeline, pc);
        cmd.setViewport({0, 0, framebuffer->size.w, framebuffer->size.h});
        cmd.bindMesh(mesh);
        cmdb.drawIndexed(mesh->indicesCount, 1, 0, 0, 0);

        cmd.endPass();
    }
};

void Camera::rotateX(float degrees) {
    auto angle = glm::radians(degrees / 2.f);
    glm::quat rotation(glm::cos(angle), glm::vec3(0, 1, 0) * glm::sin(angle));
    glm::quat rotationC = glm::conjugate(rotation);

    dir = rotation * dir * rotationC;
}
void Camera::rotateY(float degrees) {
    auto angle = glm::radians(degrees / 2.f);
    glm::quat rotation(glm::cos(angle),
                       glm::cross(dir, glm::vec3(0, 1, 0)) * glm::sin(angle));
    glm::quat rotationC = glm::conjugate(rotation);

    dir = rotation * dir * rotationC;
}

glm::mat4 Camera::getView() {
    return glm::lookAt(position, dir + position, glm::vec3(0, 1, 0));
}

glm::mat4 Camera::getProjection() {
    auto ret = glm::perspective(glm::radians(fov), w / h, 0.1f, 3000.f);
    ret[1][1] *= -1;
    return ret;
}

Engine::Engine(const RendererConfig& config, std::function<void(Engine&)> cb) {
    guiCallback = cb;
    Size winSize = {.w = config.width, .h = config.height};
    presentation = std::make_unique<Window>(winSize, "Vulkan Terrain");
    val::EngineInitConfig initConfig;
    initConfig.appName = "Vulkan Terrain";
    initConfig.presentation = val::PresentationFormat::Immediate;
    initConfig.useImGUI = true;
    initConfig.screenSize = {.w = 1920, .h = 1080};

    engine = std::make_unique<val::Engine>(initConfig, presentation.get());
    writer = std::make_unique<val::BufferWriter>(*engine);
    terrainRenderer = std::make_unique<TerrainRenderer>(*engine, *writer);
    waterRenderer = std::make_unique<WaterRenderer>(*engine, *writer);
    skyboxRenderer = std::make_unique<SkyboxRenderer>(*engine, *writer);

    frameBuffer =
        engine->createTexture(Size{1920, 1080}, val::TextureFormat::RGBA16);
    depthBuffer =
        engine->createTexture(Size{1920, 1080}, val::TextureFormat::DEPTH32);

    screenTexture =
        engine->createTexture(Size{1920, 1080}, val::TextureFormat::RGBA16,
                              val::TextureSampler::LINEAR);

    engine->createMesh(4, 1);

    time.ticks = SDL_GetTicks();

    globalData = engine->createStorageBuffer(sizeof(GlobalData));
}

Engine::~Engine() { engine->waitFinishAllCommands(); }

void Engine::updateInput() {
    auto winSize = presentation->getSize();
    glm::vec2 center(winSize.w / 2.f, winSize.h / 2.f);
    if (mouseCaputure.capture) {
        SDL_SetRelativeMouseMode(true);
        glm::vec2 mousePos;
        SDL_GetMouseState(&mousePos.x, &mousePos.y);

        mouseCaputure.mouseDelta =
            (center - mousePos) / glm::vec2((float)winSize.w, (float)winSize.h);

        SDL_WarpMouseInWindow(*presentation, center.x, center.y);
    } else {
        SDL_SetRelativeMouseMode(false);
        mouseCaputure.mouseDelta = {};
    }
    for (auto& [scan, key] : keyStates) {
        if (key == KeyState::PRESSED) key = KeyState::HOLD;
    }
    SDL_Event event;
    SDL_Scancode key;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_EVENT_QUIT:
                _shouldClose = true;
                break;
            case SDL_EVENT_KEY_DOWN:
                key = event.key.keysym.scancode;
                if (getKeyState(key) == KeyState::RELEASED) {
                    keyStates[key] = KeyState::PRESSED;
                }
                break;

            case SDL_EVENT_KEY_UP:
                key = event.key.keysym.scancode;
                keyStates.erase(key);
                break;
        }
        ImGui_ImplSDL3_ProcessEvent(&event);
    }
}

bool Engine::shouldClose() { return _shouldClose; }

void Engine::render(Camera& camera) {
    time.deltaTime = (SDL_GetTicks() - time.ticks) / 1000.f;
    time.ticks = SDL_GetTicks();
    time.time += time.deltaTime;
    engine->update();

    auto cmd = engine->initFrame();

    CameraData& cd = state.cam;
    cd.pos = camera.position;
    cd.up = glm::vec3(0, 1, 0);
    cd.dir = camera.dir;
    cd.proj = camera.getProjection();
    cd.view = camera.getView();

    auto m = glm::transpose(cd.proj * cd.view);

    cd.frustum.right = m[3] - m[0];
    cd.frustum.left = m[3] + m[0];
    cd.frustum.top = m[3] - m[1];
    cd.frustum.bottom = m[3] + m[1];
    cd.frustum.front = m[3] + m[2];
    cd.frustum.back = m[3] - m[2];

    cd.frustum.right /= glm::length(glm::vec3(cd.frustum.right));
    cd.frustum.left /= glm::length(glm::vec3(cd.frustum.left));
    cd.frustum.top /= glm::length(glm::vec3(cd.frustum.top));
    cd.frustum.bottom /= glm::length(glm::vec3(cd.frustum.bottom));
    cd.frustum.front /= glm::length(glm::vec3(cd.frustum.front));
    cd.frustum.back /= glm::length(glm::vec3(cd.frustum.back));

    GlobalData gd;
    gd.fogDensity = state.fogGradient;
    gd.fogGradient = state.fogDensity;
    gd.frustum = cd.frustum;
    gd.proj = cd.proj;
    gd.view = cd.view;
    gd.projView = cd.proj * cd.view;
    gd.invP = glm::inverse(cd.proj);
    gd.invView = glm::inverse(cd.view);
    gd.skyColor = state.skyColor;
    gd.camPos = cd.pos;
    gd.time = time.time;
    state.globalData = globalData->bindPoint;
    state.skyboxTexture = skyboxRenderer->skybox->bindPoint;

    writer->enqueueBufferWrite(globalData, &gd, 0, sizeof(GlobalData));

    if (cmd.isValid()) {
        writer->updateWrites(cmd);

        cmd.transitionTexture(frameBuffer, vk::ImageLayout::eUndefined,
                              vk::ImageLayout::eTransferDstOptimal);
        cmd.clearImage(frameBuffer->image, gd.skyColor.r, gd.skyColor.g,
                       gd.skyColor.b, gd.skyColor.a);
        cmd.transitionTexture(frameBuffer, vk::ImageLayout::eTransferDstOptimal,
                              vk::ImageLayout::eColorAttachmentOptimal);

        skyboxRenderer->renderPass(frameBuffer, state, cmd);

        terrainRenderer->renderComputePass(state, cmd, waterRenderer->vertices,
                                           waterRenderer->drawCommand);
        cmd.memoryBarrier(vk::PipelineStageFlagBits2::eComputeShader,
                          vk::AccessFlagBits2::eMemoryRead |
                              vk::AccessFlagBits2::eMemoryWrite,
                          vk::PipelineStageFlagBits2::eAllCommands,
                          vk::AccessFlagBits2::eMemoryRead);
        terrainRenderer->renderDepthPrepass(depthBuffer, state, cmd);
        cmd.memoryBarrier(vk::PipelineStageFlagBits2::eLateFragmentTests,
                          vk::AccessFlagBits2::eMemoryRead |
                              vk::AccessFlagBits2::eMemoryWrite,
                          vk::PipelineStageFlagBits2::eEarlyFragmentTests,
                          vk::AccessFlagBits2::eMemoryRead);
        terrainRenderer->renderPass(depthBuffer, frameBuffer, state, cmd);

        // Copy fb to screen texture

        cmd.transitionTexture(frameBuffer,
                              vk::ImageLayout::eColorAttachmentOptimal,
                              vk::PipelineStageFlagBits2::eLateFragmentTests,
                              vk::ImageLayout::eTransferSrcOptimal,
                              vk::PipelineStageFlagBits2::eTransfer);
        cmd.transitionTexture(screenTexture, vk::ImageLayout::eUndefined,
                              vk::PipelineStageFlagBits2::eLateFragmentTests,
                              vk::ImageLayout::eTransferDstOptimal,
                              vk::PipelineStageFlagBits2::eTransfer);

        cmd.copyTextureToTexture(frameBuffer, screenTexture);

        cmd.transitionTexture(frameBuffer, vk::ImageLayout::eTransferSrcOptimal,
                              vk::PipelineStageFlagBits2::eTransfer,
                              vk::ImageLayout::eColorAttachmentOptimal,
                              vk::PipelineStageFlagBits2::eEarlyFragmentTests);
        cmd.transitionTexture(screenTexture,
                              vk::ImageLayout::eTransferDstOptimal,
                              vk::PipelineStageFlagBits2::eTransfer,
                              vk::ImageLayout::eShaderReadOnlyOptimal,
                              vk::PipelineStageFlagBits2::eEarlyFragmentTests);

        waterRenderer->renderPass(depthBuffer, frameBuffer, screenTexture,
                                  state, cmd);

        drawGUI();

        engine->submitFrame(frameBuffer);
    }
}

void Engine::drawGUI() {
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    if (guiCallback) guiCallback(*this);

    ImGui::Render();
}
}  // namespace engine
