#include "GBuffer.h"
#include "rlgl.h"
#include <iostream>

GBuffer::GBuffer()
    : framebuffer(0), positionTexture(0), normalTexture(0), albedoSpecTexture(0), depthRenderbuffer(0), width(0), height(0) {
}

GBuffer::~GBuffer() { Destroy(); }

bool GBuffer::Init(int w, int h) {
    width = w; height = h;
    framebuffer = rlLoadFramebuffer();
    if (!framebuffer) return false;
    rlEnableFramebuffer(framebuffer);

    positionTexture = rlLoadTexture(NULL, w, h, RL_PIXELFORMAT_UNCOMPRESSED_R16G16B16, 1);
    normalTexture = rlLoadTexture(NULL, w, h, RL_PIXELFORMAT_UNCOMPRESSED_R16G16B16, 1);
    albedoSpecTexture = rlLoadTexture(NULL, w, h, RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8A8, 1);

    rlActiveDrawBuffers(3);

    rlFramebufferAttach(framebuffer, positionTexture, RL_ATTACHMENT_COLOR_CHANNEL0, RL_ATTACHMENT_TEXTURE2D, 0);
    rlFramebufferAttach(framebuffer, normalTexture, RL_ATTACHMENT_COLOR_CHANNEL1, RL_ATTACHMENT_TEXTURE2D, 0);
    rlFramebufferAttach(framebuffer, albedoSpecTexture, RL_ATTACHMENT_COLOR_CHANNEL2, RL_ATTACHMENT_TEXTURE2D, 0);

    depthRenderbuffer = rlLoadTextureDepth(w, h, true);
    rlFramebufferAttach(framebuffer, depthRenderbuffer, RL_ATTACHMENT_DEPTH, RL_ATTACHMENT_RENDERBUFFER, 0);

    bool complete = rlFramebufferComplete(framebuffer);
    rlDisableFramebuffer();

    return complete;
}

void GBuffer::Bind() { rlEnableFramebuffer(framebuffer); }
void GBuffer::Unbind() { rlDisableFramebuffer(); }

void GBuffer::Destroy() {
    if (framebuffer) rlUnloadFramebuffer(framebuffer);
    if (positionTexture) rlUnloadTexture(positionTexture);
    if (normalTexture) rlUnloadTexture(normalTexture);
    if (albedoSpecTexture) rlUnloadTexture(albedoSpecTexture);
    if (depthRenderbuffer) rlUnloadTexture(depthRenderbuffer);
    framebuffer = positionTexture = normalTexture = albedoSpecTexture = depthRenderbuffer = 0;
}
