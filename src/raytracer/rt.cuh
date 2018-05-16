#pragma once

// include appropriate GL library header first

bool CUDA_device_info();
bool CUDA_init();
void * CUDA_registerGLBuffer(GLuint glBuf);
bool CUDA_unregisterGLBuffer(void * cudaBuf);
void * CUDA_registerBuffer(void * f, std::size_t size);
bool CUDA_unregisterBuffer(void * f);
bool CUDA_render(void * cudaBuf, void * scene, int w, int h);
