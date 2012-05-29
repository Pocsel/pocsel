#include "client/game/engine/Model.hpp"
#include "client/game/engine/ModelType.hpp"

#include "tools/Math.hpp"

#include "client/resources/ResourceManager.hpp"
#include "tools/renderers/utils/texture/ITexture.hpp"

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

        std::vector<std::string> const& materials = this->_model.GetMaterials();
        for (auto it = materials.begin(), ite = materials.end(); it != ite; ++it)
        {
            this->_textures.push_back(resourceManager.GetTexture(1, *it).release());
        }

        Tools::debug << "Model::Model: New model \"" << this->_type->GetName() << "\", id: " << this->_id << std::endl;
    }

    Model::~Model()
    {
        for (auto it = this->_textures.begin(), ite = this->_textures.end(); it != ite; ++it)
        {
            Tools::Delete(*it);
        }

        Tools::debug << "Model::Model: Destroying model \"" << this->_type->GetName() << "\", id: " << this->_id << std::endl;
    }

    void Model::SetAnim(std::string const& anim)
    {
        if (this->_animations.count(anim))
            this->_curAnimation = &this->_animations[anim];
        else
            this->_curAnimation = 0;
    }

    void Model::Update(Uint32 time, float phi)
    {
        auto joints = this->_model.GetJointInfos();

        if (this->_curAnimation != 0 && this->_curAnimation->numFrames > 0)
        {
            float frameDuration = 1.0f / this->_curAnimation->frameRate;

            this->_animTime += (float)time / 1000.0f;
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

                if (
                        joints[i].name == "pelvis"
                        ||
                        joints[i].name == "neck"
                        ||
                        joints[i].name == "head"
                        ||
                        joints[i].name == "spine"
                   )
                {
                    animatedJoint0.orientation = glm::normalize(glm::angleAxis<float>(glm::degrees(phi - Tools::Math::Pi/2)/4, 0, 1, 0) * animatedJoint0.orientation);
                    animatedJoint1.orientation = glm::normalize(glm::angleAxis<float>(glm::degrees(phi - Tools::Math::Pi/2)/4, 0, 1, 0) * animatedJoint1.orientation);
                }

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
            }
        }
        else
        {
            std::vector<Tools::Models::MqmModel::FrameJoint> parents(joints.size());

            for (unsigned int i = 0; i < joints.size(); ++i)
            {
                Tools::Models::MqmModel::FrameJoint& animatedJoint = parents[i];
                animatedJoint = Tools::Models::MqmModel::FrameJoint(joints[i].position, joints[i].orientation, joints[i].size);

                if (joints[i].parent >= 0) // Has a parent joint
                {
                    Tools::Models::MqmModel::FrameJoint const& parentJoint = parents[joints[i].parent];
                    animatedJoint.position = parentJoint.position + parentJoint.orientation * (animatedJoint.position/* - joints[joints[i].parent].position*/);
                    animatedJoint.orientation = glm::normalize(parentJoint.orientation * animatedJoint.orientation);
                    //animatedJoint.size = parentJoint.size * animatedJoint.size;
                }

                if (
                        joints[i].name == "Bone"
                   )
                {
                    animatedJoint.orientation =
                        glm::normalize(
                                glm::angleAxis<float>(glm::degrees(phi - Tools::Math::Pi/2), 0, 1, 0)
                                *
                                animatedJoint.orientation
                                );
                }
                if (
                        joints[i].name == "pelvis"
                        ||
                        joints[i].name == "neck"
                        ||
                        joints[i].name == "head"
                        ||
                        joints[i].name == "spine"
                   )
                {
                    animatedJoint.orientation =
                        glm::normalize(
                                glm::angleAxis<float>(glm::degrees(phi - Tools::Math::Pi/2)/4, 0, 1, 0)
                                *
                                animatedJoint.orientation
                                );
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
                    ;
            }
        }
    }

}}}
