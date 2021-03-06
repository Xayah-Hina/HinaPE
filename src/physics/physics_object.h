#ifndef HINAPE_PHYSICS_OBJECT_H
#define HINAPE_PHYSICS_OBJECT_H

#include "physics_objects/deformable.h"
#include "physics_objects/rigidbody.h"

#include <map>
#include <variant>
#include <optional>

namespace HinaPE
{
class PhysicsObject
{
public:
    // common methods
    template<class T>
    [[nodiscard]] const T &get_object() const;
    [[nodiscard]] PhysicsObjectType get_type() const;

    // universal methods
    [[nodiscard]] Vec3 get_position() const;
    [[nodiscard]] Vec3 get_rotation() const;
    [[nodiscard]] Vec3 get_velocity() const;
    [[nodiscard]] Vec3 get_force() const;
    [[nodiscard]] float get_mass() const;
    void set_position(const Vec3 &) const;
    void set_rotation(const Vec3 &) const;
    void set_velocity(const Vec3 &) const;

    // rigidbody methods
    bool is_rigidbody();
    [[nodiscard]] RigidBodyType get_rigid_body_type() const;
    void switch_rigidbody_type(RigidBodyType to);

    // deformable methods
    bool is_deformable();
    [[nodiscard]] DeformableType get_deformable_type() const;
    const std::vector<Vec3> &dirty_pos();
    const std::vector<unsigned int> &dirty_ind();

public:
    explicit PhysicsObject(PhysicsObjectType type);
    ~PhysicsObject() = default;
    PhysicsObject(const PhysicsObject &src) = delete;
    PhysicsObject &operator=(const PhysicsObject &src) = delete;
    PhysicsObject(PhysicsObject &&src) = default;
    PhysicsObject &operator=(PhysicsObject &&src) = default;

    template<RigidBodyType T>
    explicit PhysicsObject(RigidBodyBase<T> &&rigidbody);
    template<RigidBodyType T>
    PhysicsObject &operator=(RigidBodyBase<T> &&rigidbody);
    template<DeformableType T>
    explicit PhysicsObject(DeformableBase<T> &&deformable);
    template<DeformableType T>
    PhysicsObject &operator=(DeformableBase<T> &&deformable);

private:
    std::optional<std::variant<
            RigidBodyBase<HinaPE::DYNAMIC>,
            RigidBodyBase<HinaPE::STATIC>,
            RigidBodyBase<HinaPE::KINEMATIC>,
            DeformableBase<CLOTH>,
            DeformableBase<MESH>>> physics_object_opt;
};

template<HinaPE::RigidBodyType T>
HinaPE::PhysicsObject::PhysicsObject(HinaPE::RigidBodyBase<T> &&rigidbody)
{
    physics_object_opt = std::move(rigidbody);
}

template<HinaPE::RigidBodyType T>
HinaPE::PhysicsObject &HinaPE::PhysicsObject::operator=(HinaPE::RigidBodyBase<T> &&rigidbody)
{
    physics_object_opt = std::move(rigidbody);
    return *this;
}

template<HinaPE::DeformableType T>
HinaPE::PhysicsObject::PhysicsObject(HinaPE::DeformableBase<T> &&deformable)
{
    physics_object_opt = std::move(deformable);
}

template<HinaPE::DeformableType T>
HinaPE::PhysicsObject &HinaPE::PhysicsObject::operator=(HinaPE::DeformableBase<T> &&deformable)
{
    physics_object_opt = std::move(deformable);
    return *this;
}

inline PhysicsObject::PhysicsObject(PhysicsObjectType type)
{
    switch (type)
    {
        case Rigidbody:
            physics_object_opt = RigidBodyBase<HinaPE::DYNAMIC>();
            break;
        case Deformable:
            throw std::runtime_error("DEFORMABLE NOT IMPLEMENTED");
            break;
        case Fluid:
            throw std::runtime_error("FLUID NOT IMPLEMENTED");
            break;
        case NOT_PHYSICS_OBJECT:
            physics_object_opt = std::nullopt;
            break;
    }
}

template<class T>
const T &HinaPE::PhysicsObject::get_object() const
{
    return std::get<T>(physics_object_opt.value());
}

}

#endif //HINAPE_PHYSICS_OBJECT_H
