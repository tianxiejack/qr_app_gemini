#if 0
#include"GetScreenBuffer.h"
#include"GLRender.h"

Screen_data_PBO screen_data_pbo;

void Screen_data_PBO::init_PBOf4pix_buffs(){
	glGenTextures(SCREEN_TEX_COUNT,screen_Textures);
			for(int i = 0; i < SCREEN_TEX_COUNT; i++){
				glBindTexture(GL_TEXTURE_2D, screen_Textures[i]);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
				glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,WindowWidth, WindowHeight, 0,
						GL_BGRA, GL_UNSIGNED_BYTE, data_size);
			}
			glGenBuffers(1,pixBufferObjs);
			glBindBuffer(GL_PIXEL_PACK_BUFFER,pixBufferObjs[0]);
			glBufferData(GL_PIXEL_PACK_BUFFER,sizeof(data_size),data_size,GL_DYNAMIC_COPY);
			glBindBuffer(GL_PIXEL_PACK_BUFFER,0);
}



void Screen_data_PBO::ReadBuff2PBO(int startx,int starty,int width,int height)
{
	// First bind the PBO as the pack buffer, then read the pixels directly to the PBO
	glBindBuffer(GL_PIXEL_PACK_BUFFER,pixBufferObjs[0]);
	glReadPixels(startx,starty,width,height,GL_BGRA,
								GL_UNSIGNED_BYTE,NULL);
	glBindBuffer(GL_PIXEL_PACK_BUFFER,0);

	// Next bind the PBO as the unpack buffer, then push the pixels straight into the textures
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER,pixBufferObjs[0]);

    // Setup texture unit for new blur, this gets imcremented every frame
	glActiveTexture(GL_TEXTURE0+screen_data_pbo.GetscreenTarget());
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,width,height,0,GL_RGBA,GL_UNSIGNED_BYTE,NULL);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER,0);
}




void FBO_Manger::init_RBO(int width,int height)
{
	glGenRenderbuffers(1,&renderbuffernames);
	glBindRenderbuffer(GL_RENDERBUFFER,renderbuffernames);
	glBindRenderbuffer(GL_RENDERBUFFER,renderbuffernames);
	glRenderbufferStorage(GL_RENDERBUFFER,GL_RGBA8,width,height);
	glBindRenderbuffer(GL_RENDERBUFFER,depthBufferName);
	glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH_COMPONENT32,width,height);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER,fboName);
	glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,depthBufferName);
	glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_RENDERBUFFER,renderbuffernames);




}



void FBO_Manger::CopyFromScreen(int startx,int starty,int w,int h)
{
	glReadBuffer(GL_FRONT);
	glBlitFramebuffer(startx,starty,w,h,
			startx,starty,w,h,
			GL_COLOR_BUFFER_BIT,GL_LINEAR);
}
#endif
