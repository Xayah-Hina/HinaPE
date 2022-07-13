#ifndef HINAPE_RIGIDBODY_H
#define HINAPE_RIGIDBODY_H

#include "lib/vec3.h"
#include "lib/quat.h"
#include <type_traits>

#if defined(WIN32) || defined(_WIN32) || defined(WIN64)
#define HINA_FORCE_INLINE __forceinline
#else
#define HINA_FORCE_INLINE __attribute__((always_inline))
#endif

namespace HinaPE
{
enum RigidBodyType : int
{
    DYNAMIC = 0,
    STATIC = 1,
    KINEMATIC = 2,

    NOT_RIGIDBODY = -1
};

//template<RigidBodyType Type>
//class RigidBodyBase;
//
//template<RigidBodyType FromType, RigidBodyType ResType>
//RigidBodyBase<ResType> &&switch_rigidbody_type(const RigidBodyBase<FromType> &from);

template<RigidBodyType Type>
class RigidBodyBase
{
public:
    template<RigidBodyType T = Type, typename = typename std::enable_if<T == DYNAMIC>::type>
    HINA_FORCE_INLINE RigidBodyBase<T> *add_force(const Vec3 &f);

    template<RigidBodyType T = Type, typename = typename std::enable_if<T == DYNAMIC>::type>
    HINA_FORCE_INLINE RigidBodyBase<T> *add_acceleration(const Vec3 &a);

    template<RigidBodyType T = Type, typename = typename std::enable_if<T == DYNAMIC>::type>
    HINA_FORCE_INLINE RigidBodyBase<T> *set_linear_velocity(const Vec3 &v);

    template<RigidBodyType T = Type, typename = typename std::enable_if<T == DYNAMIC>::type>
    HINA_FORCE_INLINE RigidBodyBase<T> *set_angular_velocity(const Vec3 &w);

    template<RigidBodyType T = Type, typename = typename std::enable_if<T == DYNAMIC>::type>
    HINA_FORCE_INLINE RigidBodyBase<T> *set_linear_damping(float d);

    template<RigidBodyType T = Type, typename = typename std::enable_if<T == DYNAMIC>::type>
    HINA_FORCE_INLINE RigidBodyBase<T> *set_angular_damping(float d);

    template<RigidBodyType T = Type, typename = typename std::enable_if<(T == DYNAMIC || T == KINEMATIC)>::type>
    HINA_FORCE_INLINE RigidBodyBase<T> *set_mass(float m);

    template<RigidBodyType T = Type, typename = typename std::enable_if<T == DYNAMIC>::type>
    [[nodiscard]] HINA_FORCE_INLINE Vec3 get_linear_velocity() const;

    template<RigidBodyType T = Type, typename = typename std::enable_if<T == DYNAMIC>::type>
    [[nodiscard]] HINA_FORCE_INLINE Vec3 get_angular_velocity() const;

    template<RigidBodyType T = Type, typename = typename std::enable_if<T == DYNAMIC>::type>
    [[nodiscard]] HINA_FORCE_INLINE float get_linear_damping() const;

    template<RigidBodyType T = Type, typename = typename std::enable_if<T == DYNAMIC>::type>
    [[nodiscard]] HINA_FORCE_INLINE float get_angular_damping() const;

    template<RigidBodyType T = Type, typename = typename std::enable_if<(T == DYNAMIC || T == KINEMATIC)>::type>
    [[nodiscard]] HINA_FORCE_INLINE float get_mass();

public:
    RigidBodyBase();
    ~RigidBodyBase();
    RigidBodyBase &operator=(const RigidBodyBase &src) = delete;
    RigidBodyBase(const RigidBodyBase &src) = delete;
    RigidBodyBase(RigidBodyBase &&src) noexcept;
    RigidBodyBase &operator=(RigidBodyBase &&src) noexcept;

public:
    template<RigidBodyType FromType, RigidBodyType ResType>
    friend RigidBodyBase<ResType> switch_rigidbody_type(const RigidBodyBase<FromType> &from);
    template<RigidBodyType FromType, RigidBodyType ResType>
    friend void copy_impl(typename RigidBodyBase<FromType>::Impl *from, typename RigidBodyBase<ResType>::Impl *res);

private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};

template<RigidBodyType FromType, RigidBodyType ResType>
RigidBodyBase<ResType> switch_rigidbody_type(const RigidBodyBase<FromType> &from)
{
    RigidBodyBase<ResType> res;
    copy_impl<FromType, ResType>(from.impl.get(), res.impl.get());
    return std::move(res); // you may use return res auto, compiler would help auto optimize it;
}

template<RigidBodyType Type>
RigidBodyBase<Type>::RigidBodyBase() : impl(std::make_unique<Impl>())
{}

template<RigidBodyType Type>
RigidBodyBase<Type>::~RigidBodyBase()
= default;

template<RigidBodyType Type>
RigidBodyBase<Type>::RigidBodyBase(RigidBodyBase &&src) noexcept
{ impl = std::move(src.impl); }

template<RigidBodyType Type>
RigidBodyBase<Type> &RigidBodyBase<Type>::operator=(RigidBodyBase &&src) noexcept
{
    impl = std::move(src.impl);
    return *this;
}

}

#include "rigidbody_impl.inl"


#endif //HINAPE_RIGIDBODY_H
