#include <cuda.h>
#include <cuda_gl_interop.h>
#include <cudaGL.h>

#include "rt.cuh"

#include <cassert>
#include <cstdio>

static bool CUDA_check_error(const char * errMsg)
{
    cudaError err = cudaGetLastError();
    if (err != cudaSuccess) {
        fprintf(stderr, "CUDA error: %s (%s)\n", errMsg, cudaGetErrorString(err));
        return true;
    }
    return false;
}

bool CUDA_device_info()
{
    cudaDeviceProp deviceProp = cudaDevicePropDontCare;
    int devCount = 0;
    cudaGetDeviceCount(&devCount);
    if (CUDA_check_error("failed to get device count")) {
        return false;
    }
    fprintf(stderr, "CUDA: device count: %i\n", devCount);
    for (int dev = 0; dev < devCount; ++dev) {
        cudaGetDeviceProperties(&deviceProp, dev);
        if (CUDA_check_error("failed to get device properties")) {
            return false;
        }
        fprintf(stderr,
                "CUDA device %i properties:\n"
                "name: %.256s\n"
                "totalGlobalMem: %zu bytes\n"
                "sharedMemPerBlock: %zu bytes\n"
                "regsPerBlock: %i\n"
                "warpSize: %i threads\n"
                "memPitch: %zu bytes\n"
                "maxThreadsPerBlock: %i\n"
                "maxThreadsDim: (%i,%i,%i)\n"
                "maxGridSize: (%i,%i,%i)\n"
                "totalConstMem: %zu bytes\n"
                "major: %i\n"
                "minor: %i\n"
                "clockRate: %i kHz\n"
                "textureAlignment: %zu\n"
                "deviceOverlap: %i\n"
                "multiProcessorCount: %i\n"
                "kernelExecTimeoutEnabled: %i\n"
                "integrated: %i\n"
                "canMapHostMemory: %i\n"
                "computeMode: %i\n"
                "concurrentKernels: %i\n"
                "ECCEnabled: %i\n"
                "pciBusID: %i\n"
                "pciDeviceID: %i\n"
                "pciDomainID: %i\n"
                "tccDriver: %i\n"
                "asyncEngineCount: %i\n"
                "unifiedAddressing: %i\n"
                "memoryClockRate: %i kHz\n"
                "memoryBusWidth: %i bits\n"
                "l2CacheSize: %i bytes\n"
                "maxThreadsPerMultiProcessor: %i\n"
                "streamPrioritiesSupported: %i\n"
                "globalL1CacheSupported: %i\n"
                "localL1CacheSupported: %i\n"
                "sharedMemPerMultiprocessor: %zu bytes\n"
                "regsPerMultiprocessor: %i\n"
                "managedMemory: %i\n"
                "isMultiGpuBoard: %i\n"
                "multiGpuBoardGroupID: %i\n"
                "hostNativeAtomicSupported: %i\n"
                "singleToDoublePrecisionPerfRatio: %i\n"
                "pageableMemoryAccess: %i\n"
                "concurrentManagedAccess: %i\n"
                "computePreemptionSupported: %i\n"
                "canUseHostPointerForRegisteredMem: %i\n"
                "cooperativeLaunch: %i\n"
                "cooperativeMultiDeviceLaunch: %i\n"
                "sharedMemPerBlockOptin: %zu bytes\n"
                "%c",
                dev,
                deviceProp.name + 0,
                deviceProp.totalGlobalMem,
                deviceProp.sharedMemPerBlock,
                deviceProp.regsPerBlock,
                deviceProp.warpSize,
                deviceProp.memPitch,
                deviceProp.maxThreadsPerBlock,
                deviceProp.maxThreadsDim[0], deviceProp.maxThreadsDim[1], deviceProp.maxThreadsDim[2],
                deviceProp.maxGridSize[0], deviceProp.maxGridSize[1], deviceProp.maxGridSize[2],
                deviceProp.totalConstMem,
                deviceProp.major,
                deviceProp.minor,
                deviceProp.clockRate,
                deviceProp.textureAlignment,
                deviceProp.deviceOverlap,
                deviceProp.multiProcessorCount,
                deviceProp.kernelExecTimeoutEnabled,
                deviceProp.integrated,
                deviceProp.canMapHostMemory,
                deviceProp.computeMode,
                deviceProp.concurrentKernels,
                deviceProp.ECCEnabled,
                deviceProp.pciBusID,
                deviceProp.pciDeviceID,
                deviceProp.pciDomainID,
                deviceProp.tccDriver,
                deviceProp.asyncEngineCount,
                deviceProp.unifiedAddressing,
                deviceProp.memoryClockRate,
                deviceProp.memoryBusWidth,
                deviceProp.l2CacheSize,
                deviceProp.maxThreadsPerMultiProcessor,
                deviceProp.streamPrioritiesSupported,
                deviceProp.globalL1CacheSupported,
                deviceProp.localL1CacheSupported,
                deviceProp.sharedMemPerMultiprocessor,
                deviceProp.regsPerMultiprocessor,
                deviceProp.managedMemory,
                deviceProp.isMultiGpuBoard,
                deviceProp.multiGpuBoardGroupID,
                deviceProp.hostNativeAtomicSupported,
                deviceProp.singleToDoublePrecisionPerfRatio,
                deviceProp.pageableMemoryAccess,
                deviceProp.concurrentManagedAccess,
                deviceProp.computePreemptionSupported,
                deviceProp.canUseHostPointerForRegisteredMem,
                deviceProp.cooperativeLaunch,
                deviceProp.cooperativeMultiDeviceLaunch,
                deviceProp.sharedMemPerBlockOptin,
                '\n');
    }
    return true;
}

bool CUDA_init()
{
    int dev = -1;
    cudaDeviceProp deviceProp = cudaDevicePropDontCare;
    deviceProp.canMapHostMemory = 1;
    cudaChooseDevice(&dev, &deviceProp);
    if (CUDA_check_error("failed to choose device")) {
        return false;
    }
    assert(!(dev < 0));
    fprintf(stderr, "CUDA: device %i is chosen\n", dev);
    cudaGetDeviceProperties(&deviceProp, dev);
    if (CUDA_check_error("failed to get device properties")) {
        return false;
    }
    if (!deviceProp.canMapHostMemory) {
        fprintf(stderr, "CUDA: device is unable to map host memory\n");
        return false;
    }
    cudaSetDeviceFlags(cudaDeviceMapHost);
    if (CUDA_check_error("failed to set device flags")) {
        return false;
    }
    cudaSetDevice(dev);
    if (CUDA_check_error("failed to set device")) {
        return false;
    }
    return true;
}

void * CUDA_registerGLBuffer(GLuint glBuf)
{
    cudaGraphicsResource_t cudaBuf = nullptr;
    cudaGraphicsGLRegisterBuffer(&cudaBuf, glBuf, cudaGraphicsRegisterFlagsNone);
    if (CUDA_check_error("failed to register buffer")) {
        return nullptr;
    }
    return cudaBuf;
}

bool CUDA_unregisterGLBuffer(void * cudaBuf)
{
    cudaGraphicsUnregisterResource((cudaGraphicsResource_t)cudaBuf);
    if (CUDA_check_error("failed to unregister resource for buffer")) {
        return false;
    }
    return true;
}

__global__ void run(float3 * buf, uint * scene, int w, int h)
{
    int x = __mul24(blockIdx.x, blockDim.x) + threadIdx.x;
    int y = __mul24(blockIdx.y, blockDim.y) + threadIdx.y;
    int index = w * y + x;
    int resolution = w * h;
    if (!(index < resolution)) {
        return;
    }
    float3 & p = buf[index];
    if (scene) {
        if ((scene[0] != 1) || (scene[1] != 8) || (scene[2] != 0) || (scene[3] != 42)) {
            p = {1.0f, 0.0f, 0.0f};
        } else {
            p = {0.0f, 0.0f, 1.0f};
        }
            /*
        switch ((index * 3) / resolution) {
        case 0 : p = {1.0f, 0.0f, 0.0f}; break;
        case 1 : p = {0.0f, 1.0f, 0.0f}; break;
        case 2 : p = {0.0f, 0.0f, 1.0f}; break;
        default : p = {0.0f, 0.0f, 0.0f};
        }*/
    } else {
        p = {1.0f, 1.0f, 1.0f};
    }
}

inline
int divUp(int dividend, int divisor)
{
    return (dividend + (divisor - 1)) / divisor;
}

void * CUDA_registerBuffer(void * f, std::size_t size)
{
    cudaHostRegister(f, size, cudaHostRegisterDefault);
    if (CUDA_check_error("unable to register host memory")) {
        return {};
    }
    void * p = {};
    cudaHostGetDevicePointer(&p, f, 0);
    if (CUDA_check_error("unable to get device pointer for registered host memory")) {
        CUDA_unregisterBuffer(f);
        return {};
    }
    return p;
}

bool CUDA_unregisterBuffer(void * f)
{
    cudaHostUnregister(f);
    if (CUDA_check_error("unable to unregister registered host memory")) {
        return false;
    }
    return true;
}

bool CUDA_render(void * cudaBuf, void * scene, int w, int h)
{
    cudaGraphicsMapResources(1, (cudaGraphicsResource_t *)&cudaBuf);
    if (CUDA_check_error("failed to map resource")) {
        return false;
    }
    void * devPtr = nullptr;
    std::size_t size = 0;
    cudaGraphicsResourceGetMappedPointer(&devPtr, &size, (cudaGraphicsResource_t)cudaBuf);
    if (CUDA_check_error("failed to get device pointer")) {
        return false;
    }
    assert(w * h * 3 * sizeof(GLfloat) == size);
    dim3 threadsPerBlock(16, 16);
    dim3 numBlocks(divUp(w, threadsPerBlock.x), divUp(h, threadsPerBlock.y));
    if (numBlocks.x * numBlocks.y * numBlocks.z > 0) {
        run<<< numBlocks, threadsPerBlock >>>(static_cast< float3 * >(devPtr), static_cast< uint * >(scene), w, h);
        cudaDeviceSynchronize();
        CUDA_check_error("failed to launch run() kernel");
    }
    cudaGraphicsUnmapResources(1, (cudaGraphicsResource_t *)&cudaBuf);
    if (CUDA_check_error("failed to unmap resource")) {
        return false;
    }
    return true;
}
