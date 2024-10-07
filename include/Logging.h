#ifndef logging_h
#define logging_h

#define LOG_LEVEL_DEBUG 100
#define LOG_LEVEL_INFO 200
#define LOG_LEVEL_WARNING 300
#define LOG_LEVEL_ERROR 400

#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_LEVEL_DEBUG
#endif

static const __FlashStringHelper *get_level_name(int level)
{
    switch (level)
    {
    case 200:
        return F("INFO");
    case 300:
        return F("WARNING");
    case 400:
        return F("ERROR");
    }
    return F("DEBUG");
}

#define LOGF(level, f_, ...)                                         \
    {                                                                \
        if (LOG_LEVEL <= level)                                      \
        {                                                            \
            Serial.print(F("["));                                    \
            Serial.print(get_level_name(level));                     \
            Serial.printf_P(PSTR("][%s:%d]: "), __FILE__, __LINE__); \
            Serial.printf_P(PSTR((f_)), __VA_ARGS__);                \
            Serial.println();                                        \
        }                                                            \
    }

#define LOG(level, msg)                                              \
    {                                                                \
        if (LOG_LEVEL <= level)                                      \
        {                                                            \
            Serial.print(F("["));                                    \
            Serial.print(get_level_name(level));                     \
            Serial.printf_P(PSTR("][%s:%d]: "), __FILE__, __LINE__); \
            Serial.println(F(msg));                                  \
        }                                                            \
    }

#if LOG_LEVEL <= LOG_LEVEL_DEBUG
#define DEBUG(msg)                 \
    {                              \
        LOG(LOG_LEVEL_DEBUG, msg); \
    }
#define DEBUGF(f_, ...)                         \
    {                                           \
        LOGF(LOG_LEVEL_DEBUG, f_, __VA_ARGS__); \
    }
#else
#define DEBUG(msg)
#define DEBUGF(f_, ...)
#endif

#if LOG_LEVEL <= LOG_LEVEL_INFO
#define INFO(msg)                 \
    {                             \
        LOG(LOG_LEVEL_INFO, msg); \
    }
#define INFOF(f_, ...)                         \
    {                                          \
        LOGF(LOG_LEVEL_INFO, f_, __VA_ARGS__); \
    }
#else
#define INFO(msg)
#define INFOF(f_, ...)
#endif

#if LOG_LEVEL <= LOG_LEVEL_WARNING
#define WARNING(msg)                 \
    {                                \
        LOG(LOG_LEVEL_WARNING, msg); \
    }
#define WARNINGF(f_, ...)                         \
    {                                             \
        LOGF(LOG_LEVEL_WARNING, f_, __VA_ARGS__); \
    }
#else
#define WARNING(msg)
#define WARNINGF(f_, ...)
#endif

#if LOG_LEVEL <= LOG_LEVEL_ERROR
#define ERROR(msg)                 \
    {                              \
        LOG(LOG_LEVEL_ERROR, msg); \
    }
#define ERRORF(f_, ...)                         \
    {                                           \
        LOGF(LOG_LEVEL_ERROR, f_, __VA_ARGS__); \
    }
#else
#define ERROR(msg)
#define ERRORF(f_, ...)
#endif

#endif