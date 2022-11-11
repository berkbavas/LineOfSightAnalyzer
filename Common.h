#ifndef COMMON_H
#define COMMON_H

#define DEFINE_MEMBER(type, name) \
protected: \
    type m##name; \
\
public: \
    inline const type &get##name(void) const { return m##name; } \
\
public: \
    inline type &get##name##_nonConst(void) { return m##name; } \
\
public: \
    inline void set##name(const type &var) { m##name = var; }

#define DEFINE_MEMBER_CONST(type, name) \
protected: \
    type m##name; \
\
public: \
    inline const type &get##name(void) const { return m##name; }

enum class ShaderType { //
    None,
    TerrainShader,
    ScreenShader,
    ObserverShader,
    ObserverComputeShader
};

#endif // COMMON_H
