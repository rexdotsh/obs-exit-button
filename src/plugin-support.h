#pragma once

#ifdef __cplusplus
extern "C" {
#endif

extern const char *PLUGIN_NAME;
extern const char *PLUGIN_VERSION;

void plugin_log(int log_level, const char *format, ...);

#ifdef __cplusplus
}
#endif
