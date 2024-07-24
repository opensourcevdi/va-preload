#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>
#include <va/va.h>

#define PRELOAD(sym_t, sym)                                                    \
  static sym_t (*_##sym)();                                                    \
  __attribute__((constructor)) static void _##sym##_init() {                   \
    _##sym = dlsym(RTLD_NEXT, #sym);                                           \
  }                                                                            \
  sym_t(sym)

static VABufferID buf_id2;

PRELOAD(VAStatus, vaCreateBuffer)
(VADisplay dpy, VAContextID context, VABufferType type, unsigned int size,
 unsigned int num_elements, void *data, VABufferID *buf_id) {
  VAStatus ret =
      _vaCreateBuffer(dpy, context, type, size, num_elements, data, buf_id);
  if (type == VAProcPipelineParameterBufferType)
    buf_id2 = *buf_id;
  return ret;
}

PRELOAD(VAStatus, vaUnmapBuffer)(VADisplay dpy, VABufferID buf_id) {
  VAStatus ret = _vaUnmapBuffer(dpy, buf_id);
  if (buf_id == buf_id2) {
    VAProcPipelineParameterBuffer *pipeline_param;
    vaMapBuffer(dpy, buf_id, (void **)&pipeline_param);

    pipeline_param->filter_flags |= VA_FILTER_INTERPOLATION_NEAREST_NEIGHBOR;
    __builtin_dump_struct(pipeline_param, &printf);

    _vaUnmapBuffer(dpy, buf_id);
  }
  buf_id2 = 0;
  return ret;
}
