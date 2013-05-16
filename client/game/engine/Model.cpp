#include "client/precompiled.hpp"

#include "client/game/engine/Model.hpp"
#include "client/game/engine/ModelType.hpp"

#include "tools/Math.hpp"

#include "client/resources/ResourceManager.hpp"
#include "tools/renderers/utils/texture/ITexture.hpp"

#include "common/physics/Node.hpp"

namespace Client { namespace Game { namespace Engine {

    Model::Model(Resources::ResourceManager& resourceManager, Uint32 id, Uint32 doodadId, Doodad* doodad, ModelType* type) :
        _type(type),
        _id(id),
        _doodadId(doodadId),
        _doodad(doodad),
        _model(resourceManager.GetMqmModel(type->GetResourceId())),
        _animTime(0)
    {
        auto anims = this->_model.GetAnimInfos();
        for (auto it = anims.begin(), ite = anims.end(); it != ite; ++it)
        {
            this->_animations[it->name] = *it;
        }
        this->SetAnim("idle");

        this->_animatedBones.assign(this->_model.GetJointInfos().size(), glm::mat4x4(1));
        if (this->_model.GetJointInfos().size() == 0)
            this->_animatedBones.assign(1, glm::mat4x4(1));

        this->_boundBones.resize(this->_model.GetJointInfos().size());
        if (this->_model.GetJointInfos().size() == 0)
            this->_boundBones.resize(1);

        std::vector<std::string> const& materials = this->_model.GetMaterials();
        for (auto it = materials.begin(), ite = materials.end(); it != ite; ++it)
            this->_materials.push_back(resourceManager.GetMaterial(*it));

        Tools::debug << "Model::Model: New model \"" << this->_type->GetName() << "\", id: " << this->_id << std::endl;
    }

    Model::~Model()
    {
        Tools::debug << "Model::Model: Destroying model \"" << this->_type->GetName() << "\", id: " << this->_id << std::endl;
    }

    void Model::SetAnim(std::string const& anim)
    {
        if (this->_animations.count(anim))
            this->_curAnimation = &this->_animations[anim];
        else
            this->_curAnimation = 0;
    }

    void Model::Update(float deltaTime, float phi)
    {
        auto joints = this->_model.GetJointInfos();

        if (this->_curAnimation != 0 && this->_curAnimation->numFrames > 0)
        {
            float frameDuration = 1.0f / this->_curAnimation->frameRate;

            this->_animTime += deltaTime;
            this->_animTime = std::fmod(this->_animTime, this->_curAnimation->numFrames * frameDuration);

            float interpolate = std::fmod(_animTime, frameDuration) * this->_curAnimation->frameRate;

            int frame1 = (int)(this->_animTime * this->_curAnimation->frameRate) % this->_curAnimation->numFrames;
            int frame2 = (frame1 + 1) % this->_curAnimation->numFrames;
            std::vector<Tools::Models::MqmModel::FrameJoint> const& frames1 = this->_model.GetFrames()[frame1 + this->_curAnimation->firstFrame];
            std::vector<Tools::Models::MqmModel::FrameJoint> const& frames2 = this->_model.GetFrames()[frame2 + this->_curAnimation->firstFrame];

            std::vector<Tools::Models::MqmModel::FrameJoint> parents0(joints.size());
            std::vector<Tools::Models::MqmModel::FrameJoint> parents1(joints.size());

            for (unsigned int i = 0; i < joints.size(); ++i)
            {
                Tools::Models::MqmModel::FrameJoint& animatedJoint0 = parents0[i];
                animatedJoint0 = frames1[i];
                Tools::Models::MqmModel::FrameJoint& animatedJoint1 = parents1[i];
                animatedJoint1 = frames2[i];

                if (joints[i].parent >= 0) // Has a parent joint
                {
                    Tools::Models::MqmModel::FrameJoint const& parentJoint0 = parents0[joints[i].parent];
                    animatedJoint0.position = parentJoint0.position + parentJoint0.orientation * animatedJoint0.position;
                    animatedJoint0.orientation = glm::normalize(parentJoint0.orientation * animatedJoint0.orientation);
                    animatedJoint0.size = parentJoint0.size * animatedJoint0.size;

                    Tools::Models::MqmModel::FrameJoint const& parentJoint1 = parents1[joints[i].parent];
                    animatedJoint1.position = parentJoint1.position + parentJoint1.orientation * animatedJoint1.position;
                    animatedJoint1.orientation = glm::normalize(parentJoint1.orientation * animatedJoint1.orientation);
                    animatedJoint1.size = parentJoint1.size * animatedJoint1.size;
                }

                //if (
                //        joints[i].name == "pelvis"
                //        ||
                //        joints[i].name == "neck"
                //        ||
                //        joints[i].name == "head"
                //        ||
                //        joints[i].name == "spine"
                //   )
                //{
                //    animatedJoint0.orientation = glm::normalize(glm::angleAxis(glm::degrees(phi - Tools::Math::PiFloat/2.0f)/4.0f, 0.0f, 1.0f, 0.0f) * animatedJoint0.orientation);
                //    animatedJoint1.orientation = glm::normalize(glm::angleAxis(glm::degrees(phi - Tools::Math::PiFloat/2.0f)/4.0f, 0.0f, 1.0f, 0.0f) * animatedJoint1.orientation);
                //}

                this->_animatedBones[i] =
                    (
                     glm::scale(
                         glm::lerp(animatedJoint0.size, animatedJoint1.size, interpolate)
                         )
                     *
                     glm::translate(
                         glm::lerp(animatedJoint0.position, animatedJoint1.position, interpolate) // finalPos
                         )
                     *
                     glm::toMat4(
                         glm::shortMix(animatedJoint0.orientation, animatedJoint1.orientation, interpolate) // finalOrient
                         )
                    )
                    *
                    this->_model.GetInverseBindPose()[i]
                    ;

              //      if (this->_boundBones[i])
              //          this->_animatedBones[i] *= *this->_boundBones[i];
            }
        }
        else
        {
            std::vector<Tools::Models::MqmModel::FrameJoint> parents(joints.size());

            for (unsigned int i = 0; i < joints.size(); ++i)
            {
                Tools::Models::MqmModel::FrameJoint& animatedJoint = parents[i];
                animatedJoint = Tools::Models::MqmModel::FrameJoint(joints[i].position, joints[i].orientation, joints[i].size);

                // glm::mat4x4 toto(1);

                if (this->_boundBones[i])
                {
                    glm::vec3 p(this->_boundBones[i]->position);
                    glm::quat r(
                            this->_boundBones[i]->orientation.w,
                            this->_boundBones[i]->orientation.x,
                            this->_boundBones[i]->orientation.y,
                            this->_boundBones[i]->orientation.z
                            );
                    // toto = glm::translate(p) * glm::toMat4(r);

                    //p = (this->_model.GetBindPose()[i] * glm::translate(p));
                    r = glm::toQuat(this->_model.GetBindPose()[i]) * glm::normalize(r);
                    animatedJoint.position =
                        animatedJoint.position + animatedJoint.orientation * p;

                    // XXX chelouxe
                    animatedJoint.orientation =
                        glm::normalize(
                                glm::toQuat(this->_model.GetInverseBindPose()[i])
                                *
                                glm::normalize(r)
                                *
                                animatedJoint.orientation
                                );
                }

                if (joints[i].parent >= 0) // Has a parent joint
                {
                    Tools::Models::MqmModel::FrameJoint const& parentJoint = parents[joints[i].parent];
                    animatedJoint.position = parentJoint.position + parentJoint.orientation * (animatedJoint.position/* - joints[joints[i].parent].position*/);
                    animatedJoint.orientation = glm::normalize(parentJoint.orientation * animatedJoint.orientation);
                    //animatedJoint.size = parentJoint.size * animatedJoint.size;
                }

                this->_animatedBones[i] =
                    (
                     //glm::scale(animatedJoint.size)
                     //*
                     glm::translate(animatedJoint.position)
                     *
                     glm::toMat4(animatedJoint.orientation)
                    )
                    *
                    this->_model.GetInverseBindPose()[i]
                    //*
                    //toto
                    ;

              //  if (this->_boundBones[i])
              //      this->_animatedBones[i] *= *this->_boundBones[i];
            }
        }
    }

    bool Model::BindBone(std::string const& boneName, std::shared_ptr<Common::Physics::Node> const& boundBone)
    {
        unsigned int idx = 0;
        for (auto& jointInfo: this->_model.GetJointInfos())
        {
            if (jointInfo.name == boneName)
            {
                this->_boundBones[idx] = boundBone;
                return true;
            }
            ++idx;
        }
        return false;
    }

}}}
