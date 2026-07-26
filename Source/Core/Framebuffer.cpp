#include "Framebuffer.h"

#include "Util/Logger.h"

LineOfSightAnalyzer::Framebuffer::Framebuffer(int Width, int Height, const QOpenGLFramebufferObjectFormat &Format, const QVector<GLenum> &ExtraColorAttachments)
    : mWidth(Width)
    , mHeight(Height)
    , mFramebufferFormat(Format)
{
    initializeOpenGLFunctions(); // Initialize OpenGL functions for this class

    mDrawBuffers.push_back(FBO_ATTACHMENTS[0]); // Default to the first color attachment
    mNumColorAttachments = 1;                   // Start with one color attachment

    for (const auto &InternalFormat : ExtraColorAttachments)
    {
        AddColorAttachment(InternalFormat); // Add any extra color attachments specified
    }

    Resize(Width, Height); // Create the framebuffer with the specified width and height
}

void LineOfSightAnalyzer::Framebuffer::AddColorAttachment(GLenum InternalFormat)
{
    LOS_EXIT_FAILURE_IF(mNumColorAttachments >= 8, "Framebuffer::AddColorAttachment: Cannot add more than 8 color attachments.");

    const auto NextAttachment = FBO_ATTACHMENTS[mNumColorAttachments];

    mExtraColorAttachments[NextAttachment] = InternalFormat; // Store the internal format for this attachment
    mDrawBuffers.push_back(NextAttachment);                  // Add the corresponding draw buffer
    mNumColorAttachments++;
}

void LineOfSightAnalyzer::Framebuffer::Resize(int Width, int Height)
{
    mWidth = Width;
    mHeight = Height;

    if (mFramebuffer)
    {
        mFramebuffer->release(); // Release the existing framebuffer before resizing
    }

    // Create a new framebuffer with the updated size and the same format
    mFramebuffer = std::make_unique<QOpenGLFramebufferObject>(Width, Height, mFramebufferFormat);

    // Re-add extra color attachments if any were added previously
    for (const auto &[Attachment, InternalFormat] : mExtraColorAttachments)
    {
        mFramebuffer->addColorAttachment(Width, Height, InternalFormat);
    }

    mFramebuffer->bind();
    glDrawBuffers(static_cast<GLsizei>(mDrawBuffers.size()), mDrawBuffers.data());
    mFramebuffer->release();
}

void LineOfSightAnalyzer::Framebuffer::Bind()
{
    mFramebuffer->bind();
    glDrawBuffers(static_cast<GLsizei>(mDrawBuffers.size()), mDrawBuffers.data());
}

void LineOfSightAnalyzer::Framebuffer::Unbind()
{
    Release();
}

void LineOfSightAnalyzer::Framebuffer::Release()
{
    mFramebuffer->release();
}

void LineOfSightAnalyzer::Framebuffer::ClearDepthBuffer()
{
    mFramebuffer->bind();
    glDrawBuffers(static_cast<GLsizei>(mDrawBuffers.size()), mDrawBuffers.data());
    glClear(GL_DEPTH_BUFFER_BIT);
    mFramebuffer->release();
}

QOpenGLFramebufferObject *LineOfSightAnalyzer::Framebuffer::GetFramebufferObject() const
{
    return mFramebuffer.get();
}

const QOpenGLFramebufferObjectFormat &LineOfSightAnalyzer::Framebuffer::GetFramebufferFormat() const
{
    return mFramebufferFormat;
}

GLuint LineOfSightAnalyzer::Framebuffer::GetTexture() const
{
    return mFramebuffer ? mFramebuffer->texture() : 0;
}

QList<GLuint> LineOfSightAnalyzer::Framebuffer::GetTextures() const
{
    return mFramebuffer ? mFramebuffer->textures() : QList<GLuint>();
}

GLuint LineOfSightAnalyzer::Framebuffer::GetHandle() const
{
    return mFramebuffer ? mFramebuffer->handle() : 0;
}

int LineOfSightAnalyzer::Framebuffer::GetWidth() const
{
    return mWidth;
}

int LineOfSightAnalyzer::Framebuffer::GetHeight() const
{
    return mHeight;
}

QRect LineOfSightAnalyzer::Framebuffer::GetViewport() const
{
    return QRect(0, 0, mWidth, mHeight);
}

void LineOfSightAnalyzer::Framebuffer::BlitDepthTo(Framebuffer *pTargetFramebuffer)
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, GetHandle());
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, pTargetFramebuffer->GetHandle());

    glBlitFramebuffer(0,
                      0,
                      mWidth,
                      mHeight, // Source bounds
                      0,
                      0,
                      pTargetFramebuffer->GetWidth(),
                      pTargetFramebuffer->GetHeight(), // Destination bounds
                      GL_DEPTH_BUFFER_BIT,
                      GL_NEAREST);
}

void LineOfSightAnalyzer::Framebuffer::BlitColorBufferTo(Framebuffer *pTargetFramebuffer, GLuint Attachment)
{
    QOpenGLFramebufferObject::blitFramebuffer( //
        pTargetFramebuffer->GetFramebufferObject(),
        pTargetFramebuffer->GetViewport(),
        mFramebuffer.get(),
        GetViewport(),
        GL_COLOR_BUFFER_BIT,
        GL_LINEAR,
        Attachment - GL_COLOR_ATTACHMENT0,
        Attachment - GL_COLOR_ATTACHMENT0);
}
