// ReSharper disable CppTooWideScopeInitStatement
#include "openVCB.hh"
#include <tbb/spin_mutex.h>

#if defined _MSC_VER
# define EXPORT_API __declspec(dllexport)
#else
# define EXPORT_API
#endif

namespace {
/****************************************************************************************/

using openVCB::Ink, openVCB::Logic;

constexpr float TARGET_DT = 1.f / 60;
constexpr float MIN_DT    = 1.f / 30;

openVCB::Project *proj      = nullptr;
std::thread      *simThread = nullptr;
tbb::spin_mutex   simLock;

float  targetTPS  = 0;
double maxTPS     = 0;
bool   run        = true;
bool   breakpoint = false;


[[__gnu__::__hot__]]
void
simFunc()
{
      using clock = std::chrono::high_resolution_clock;
      using std::chrono::duration, std::chrono::milliseconds;

      double tpsEst       = 2 / TARGET_DT;
      double desiredTicks = 0;
      auto   lastTime     = clock::now();


      while (run) {
            auto curTime = clock::now();
            desiredTicks = std::min(desiredTicks + duration_cast<duration<double>>(curTime - lastTime).count() * targetTPS, tpsEst * MIN_DT);
            lastTime     = curTime;

            // Find max tick amount we can do
            if (desiredTicks >= 1.) {
                  double     maxTickAmount = std::max(tpsEst * TARGET_DT, 1.);
                  auto const tickAmount    = static_cast<int32_t>(std::min(desiredTicks, maxTickAmount));

                  // Aquire lock, simulate, and time
                  simLock.lock();
                  auto t1  = clock::now();
                  auto res = proj->tick(tickAmount, INT64_C(100'000'000));
                  auto t2  = clock::now();
                  simLock.unlock();

                  // Use timings to estimate max possible tps
                  desiredTicks = desiredTicks - res.numTicksProcessed;
                  maxTPS       = res.numTicksProcessed / duration_cast<duration<double>>(t2 - t1).count();
                  if (isfinite(maxTPS))
                        tpsEst = glm::clamp(glm::mix(maxTPS, tpsEst, 0.95), 1., 1e8);

                  if (res.breakpoint) {
                        targetTPS    = 0;
                        desiredTicks = 0;
                        breakpoint   = true;
                  }
            }

            // Check how much time I got left and sleep until the next check
            curTime       = clock::now();
            auto const ms = 1000 * (TARGET_DT - duration_cast<duration<double>>(curTime - lastTime).count());
            if (ms > 1.05)
                  std::this_thread::sleep_for(milliseconds{static_cast<uint64_t>(ms)});
      }
}


} // namespace
/****************************************************************************************/


extern "C" {

/*
 * Functions to control openVCB simulations
 */

EXPORT_API long long
getLineNumber(int const addr)
{
      auto const itr = proj->lineNumbers.find(addr);
      if (itr != proj->lineNumbers.end())
            return itr->second;
      return 0;
}

EXPORT_API size_t
getSymbol(char const *buf, int const size)
{
      auto const itr = proj->assemblySymbols.find({buf, static_cast<size_t>(size)});
      if (itr != proj->assemblySymbols.end())
            return itr->second;
      return 0;
}

EXPORT_API size_t
getNumTicks()
{
      return proj->tickNum;
}

EXPORT_API float
getMaxTPS()
{
      return static_cast<float>(maxTPS);
}

EXPORT_API uint32_t
getVMemAddress()
{
      return proj->lastVMemAddr;
}

EXPORT_API void
setTickRate(float const tps)
{
      targetTPS = std::max(0.0f, tps);
}

EXPORT_API void
tick(int const tick)
{
      float const tps = targetTPS;
      targetTPS       = 0;
      simLock.lock();
      proj->tick(tick);
      simLock.unlock();
      targetTPS = tps;
}

EXPORT_API void
toggleLatch(int const x, int const y)
{
      float const tps = targetTPS;
      targetTPS       = 0;
      simLock.lock();
      proj->toggleLatch(glm::ivec2(x, y));
      simLock.unlock();
      targetTPS = tps;
}

EXPORT_API void
toggleLatchIndex(int const idx)
{
      float const tps = targetTPS;
      targetTPS       = 0;
      simLock.lock();
      proj->toggleLatch(idx);
      simLock.unlock();
      targetTPS = tps;
}

EXPORT_API void
addBreakpoint(int gid)
{
      float const tps = targetTPS;
      targetTPS       = 0;
      simLock.lock();
      proj->addBreakpoint(gid);
      simLock.unlock();
      targetTPS = tps;
}

EXPORT_API void
removeBreakpoint(int gid)
{
      float const tps = targetTPS;
      targetTPS       = 0;
      simLock.lock();
      proj->removeBreakpoint(gid);
      simLock.unlock();
      targetTPS = tps;
}

EXPORT_API int
pollBreakpoint()
{
      bool const res = breakpoint;
      breakpoint     = false;
      return res;
}

EXPORT_API void
setClockPeriod(unsigned long long period)
{
      proj->clockPeriod = period;
}


/*--------------------------------------------------------------------------------------*/
/*
 * Functions to initialize openVCB
 */

EXPORT_API void
newProject()
{
      proj = new openVCB::Project;
}

EXPORT_API int
initProject()
{
      proj->preprocess();

      // Start sim thread paused
      run       = true;
      simThread = new std::thread(simFunc);

      return proj->numGroups;
}

EXPORT_API void
initVMem(char const *assembly, int const aSize, char *err, int const errSize)
{
      proj->assembly = std::string(assembly, aSize);
      //proj->my_assembleVmem(err, errSize);
      proj->assembleVmem(err, errSize);
}

EXPORT_API void
deleteProject()
{
      run = false;
      if (simThread) {
            simThread->join();
            delete simThread;
            simThread = nullptr;
      }

      if (proj) {
            // These should be managed.
            proj->states = nullptr;
            proj->vmem   = nullptr;
            proj->image  = nullptr;

            delete proj;
            proj = nullptr;
      }
}

/*--------------------------------------------------------------------------------------*/
/*
 * Functions to replace openVCB buffers with managed ones
 */

EXPORT_API void
addInstrumentBuffer(openVCB::InkState *buf, int const bufSize, int const idx)
{
      float const tps = targetTPS;
      targetTPS       = 0;
      simLock.lock();
      proj->instrumentBuffers.push_back({buf, bufSize, idx});
      simLock.unlock();
      targetTPS = tps;
}

EXPORT_API void
setStateMemory(int *data, int const size)
{
      memcpy(data, proj->states, sizeof(int) * size);
      delete[] proj->states;
      proj->states = reinterpret_cast<openVCB::InkState *>(data);
}

EXPORT_API void
setVMemMemory(int *data, int const size)
{
#ifdef OVCB_BYTE_ORIENTED_VMEM
      proj->vmem.b = reinterpret_cast<uint8_t *>(data);
#else
      proj->vmem.i = reinterpret_cast<uint32_t *>(data);
#endif
      proj->vmemSize = size;
}

EXPORT_API void
setIndicesMemory(int *data, int const size)
{
      memcpy(data, proj->indexImage, sizeof(int) * size);
}

EXPORT_API void
setImageMemory(int *data, int const width, int const height)
{
      proj->width  = width;
      proj->height = height;
      proj->image  = reinterpret_cast<openVCB::InkPixel *>(data);
}

EXPORT_API void
setDecoMemory(_Inout_ int *__restrict const indices,
              UU      int const indLen,
              _In_    int const *__restrict const col,
              UU      int const colLen)
{
      std::vector<bool>      visited(proj->width * proj->height, false);
      std::queue<glm::ivec3> queue;

      for (int32_t y = 0; y < proj->height; ++y)
            for (int32_t x = 0; x < proj->width; ++x) {
                  int32_t const idx = x + y * proj->width;
                  indices[idx]      = -1;

                  if (static_cast<uint32_t>(col[idx]) != UINT32_C(0xFFFFFFFF))
                        continue;

                  auto const ink = setOff(proj->image[idx].ink);
                  if (util::eq_any(ink, Ink::LatchOff, Ink::LedOff)) {
                        queue.push(glm::ivec3(x, y, proj->indexImage[idx]));
                        visited[idx] = true;
                  }
            }

      constexpr glm::ivec2 fourNeighbors[] = {
            glm::ivec2{-1, 0}, glm::ivec2{0,  1},
            glm::ivec2{ 1, 0}, glm::ivec2{0, -1}
      };

      while (!queue.empty()) {
            auto pos = queue.front();
            queue.pop();

            indices[pos.x + pos.y * proj->width] = pos.z;

            for (auto const &neighbor : fourNeighbors) {
                  glm::ivec2 const np = static_cast<glm::ivec2>(pos) + neighbor;
                  if (np.x < 0 || np.x >= proj->width || np.y < 0 || np.y >= proj->height)
                        continue;

                  int const nidx = np.x + np.y * proj->width;
                  if (visited[nidx])
                        continue;

                  visited[nidx] = true;

                  if (static_cast<unsigned>(col[nidx]) != 0xFFFFFFFFU)
                        continue;
                  queue.push({np.x, np.y, pos.z});
            }
      }
}

/*--------------------------------------------------------------------------------------*/
/*
 * Functions to configure openVCB
 */

EXPORT_API void
getGroupStats(int *numGroups, int *numConnections)
{
      *numGroups      = proj->numGroups;
      *numConnections = proj->writeMap.nnz;
}

EXPORT_API void
setInterface(openVCB::LatchInterface addr, openVCB::LatchInterface data)
{
      proj->vmAddr = addr;
      proj->vmData = data;
}


/*--------------------------------------------------------------------------------------*/
} // extern "C"