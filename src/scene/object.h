#ifndef HINAPE_SCENE_OBJECT_H
#define HINAPE_SCENE_OBJECT_H

#include "../geometry/halfedge.h"
#include "../platform/gl.h"
#include "../rays/shapes.h"
#include "../physics/rigidbody.h"

#include "material.h"
#include "pose.h"
#include "skeleton.h"

using Scene_ID = unsigned int;
constexpr int MAX_NAME_LEN = 256;

namespace PT
{
template<typename T>
class BVH;
class Object;
} // namespace PT

class Scene_Object
{
public:
    Scene_Object() = default;
    Scene_Object(Scene_ID id, Pose pose, GL::Mesh &&mesh, std::string n = {}, bool is_rigidbody = false);
    Scene_Object(Scene_ID id, Pose pose, Halfedge_Mesh &&mesh, std::string n = {}, bool is_rigidbody = false);
    Scene_Object(const Scene_Object &src) = delete;
    Scene_Object(Scene_Object &&src) = default;
    ~Scene_Object() = default;

    void operator=(const Scene_Object &src) = delete;
    Scene_Object &operator=(Scene_Object &&src) = default;

    Scene_ID id() const;
    void sync_mesh();
    void sync_anim_mesh();
    void set_time(float time);

    const GL::Mesh &mesh();
    const GL::Mesh &posed_mesh();

    void render(const Mat4 &view, bool solid = false, bool depth_only = false, bool posed = true,
                bool anim = true);

    Halfedge_Mesh &get_mesh();
    const Halfedge_Mesh &get_mesh() const;
    void copy_mesh(Halfedge_Mesh &out);
    void take_mesh(Halfedge_Mesh &&in);
    void set_mesh(Halfedge_Mesh &in);
    Halfedge_Mesh::ElementRef set_mesh(Halfedge_Mesh &in, unsigned int eid);

    BBox bbox();
    bool is_editable() const;
    bool is_shape() const;
    bool is_rigidbody() const;
    void try_make_editable(PT::Shape_Type prev = PT::Shape_Type::none);
    void flip_normals();

    void set_mesh_dirty();
    void set_skel_dirty();
    void set_pose_dirty();

    void step(const PT::Object &scene, float dt)
    {
        if (rigidbody_opt.has_value())
        {
            if (opt.rigidbody != opt.old_rigidbody)
            {
                std::visit(
                        overloaded{[&](HinaPE::RigidBodyBase<HinaPE::DYNAMIC> &dyn)
                                   {
                                       switch (opt.rigidbody)
                                       {
                                           case HinaPE::NOT_RIGIDBODY:
                                               rigidbody_opt = std::nullopt;
                                               break;
                                           case HinaPE::STATIC:
                                               rigidbody_opt = HinaPE::switch_rigidbody_type<HinaPE::DYNAMIC, HinaPE::STATIC>(std::get<HinaPE::RigidBodyBase<HinaPE::DYNAMIC>>(rigidbody_opt.value()));
                                               break;
                                           case HinaPE::KINEMATIC:
                                               rigidbody_opt = HinaPE::switch_rigidbody_type<HinaPE::DYNAMIC, HinaPE::KINEMATIC>(std::get<HinaPE::RigidBodyBase<HinaPE::DYNAMIC>>(rigidbody_opt.value()));
                                               break;
                                           default:
                                               throw std::runtime_error("invalid rigidbody type");
                                       }
                                   },
                                   [&](HinaPE::RigidBodyBase<HinaPE::STATIC> &sta)
                                   {
                                       switch (opt.rigidbody)
                                       {
                                           case HinaPE::NOT_RIGIDBODY:
                                               rigidbody_opt = std::nullopt;
                                               break;
                                           case HinaPE::DYNAMIC:
                                               rigidbody_opt = HinaPE::switch_rigidbody_type<HinaPE::STATIC, HinaPE::DYNAMIC>(std::get<HinaPE::RigidBodyBase<HinaPE::STATIC>>(rigidbody_opt.value()));
                                               break;
                                           case HinaPE::KINEMATIC:
                                               rigidbody_opt = HinaPE::switch_rigidbody_type<HinaPE::STATIC, HinaPE::KINEMATIC>(std::get<HinaPE::RigidBodyBase<HinaPE::STATIC>>(rigidbody_opt.value()));
                                               break;
                                           default:
                                               throw std::runtime_error("invalid rigidbody type");
                                       }
                                   },
                                   [&](HinaPE::RigidBodyBase<HinaPE::KINEMATIC> &kin)
                                   {
                                       switch (opt.rigidbody)
                                       {
                                           case HinaPE::NOT_RIGIDBODY:
                                               rigidbody_opt = std::nullopt;
                                               break;
                                           case HinaPE::DYNAMIC:
                                               rigidbody_opt = HinaPE::switch_rigidbody_type<HinaPE::KINEMATIC, HinaPE::DYNAMIC>(std::get<HinaPE::RigidBodyBase<HinaPE::KINEMATIC>>(rigidbody_opt.value()));
                                               break;
                                           case HinaPE::STATIC:
                                               rigidbody_opt = HinaPE::switch_rigidbody_type<HinaPE::KINEMATIC, HinaPE::STATIC>(std::get<HinaPE::RigidBodyBase<HinaPE::KINEMATIC>>(rigidbody_opt.value()));
                                               break;
                                           default:
                                               throw std::runtime_error("invalid rigidbody type");
                                       }
                                   }},
                        rigidbody_opt.value());
            }
        }
    }

    struct Options
    {
        char name[MAX_NAME_LEN] = {};
        bool wireframe = false;
        bool smooth_normals = false;
        bool render = true;
        PT::Shape_Type shape_type = PT::Shape_Type::none;
        PT::Shape shape;
        HinaPE::RigidBodyType rigidbody = HinaPE::NOT_RIGIDBODY;
        HinaPE::RigidBodyType old_rigidbody = HinaPE::NOT_RIGIDBODY;
    };

    Options opt;
    Pose pose;
    Anim_Pose anim;
    Skeleton armature;
    Material material;
    std::optional<
            std::variant<
                    HinaPE::RigidBodyBase<HinaPE::DYNAMIC>,
                    HinaPE::RigidBodyBase<HinaPE::STATIC>,
                    HinaPE::RigidBodyBase<HinaPE::KINEMATIC>>> rigidbody_opt;

    mutable bool rig_dirty = false;

private:
    Scene_ID _id = 0;
    Halfedge_Mesh halfedge;

    mutable GL::Mesh _mesh, _anim_mesh;
    mutable std::vector<std::vector<Joint *>> vertex_joints;
    mutable bool editable = true;
    mutable bool mesh_dirty = false;
    mutable bool skel_dirty = false, pose_dirty = false;
};

bool operator!=(const Scene_Object::Options &l, const Scene_Object::Options &r);

#endif
