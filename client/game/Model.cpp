#include "client/game/Model.hpp"

#include "client/resources/Md5Animation.hpp"
#include "client/resources/LocalResourceManager.hpp"

namespace Client { namespace Game {

    Model::Model(Resources::LocalResourceManager& resourceManager) :
        _model(resourceManager.GetMd5Model("boblampclean")),
        _animTime(0)
    {
        auto anims = this->_model.GetAnimInfos();
        for (auto it = anims.begin(), ite = anims.end(); it != ite; ++it)
        {
            this->_animations[it->name] = *it;
        std::cout << "anim: " << it->name << "\n";
        }
        this->SetAnim("idle");

        this->_animatedBones.assign(this->_model.GetJointInfos().size(), glm::mat4x4(1));
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

            float interpolate = std::fmod(_animTime, frameDuration) / frameDuration;//* this->_curAnimation->frameRate;

            int frame1 = (int)(this->_animTime * this->_curAnimation->frameRate) % this->_curAnimation->numFrames;
            int frame2 = (frame1 + 1) % this->_curAnimation->numFrames;
            std::vector<Resources::Md5Model::FrameJoint> const& frames1 = this->_model.GetFrames()[frame1 + this->_curAnimation->firstFrame];
            std::vector<Resources::Md5Model::FrameJoint> const& frames2 = this->_model.GetFrames()[frame2 + this->_curAnimation->firstFrame];

            std::vector<Resources::Md5Model::FrameJoint> parents0(joints.size());
            std::vector<Resources::Md5Model::FrameJoint> parents1(joints.size());

            for (unsigned int i = 0; i < joints.size(); ++i)
            {
                Resources::Md5Model::FrameJoint& animatedJoint0 = parents0[i];
                animatedJoint0 = frames1[i];
                Resources::Md5Model::FrameJoint& animatedJoint1 = parents1[i];
                animatedJoint1 = frames2[i];

                if (joints[i].parent >= 0) // Has a parent joint
                {
                    Resources::Md5Model::FrameJoint const& parentJoint0 = parents0[joints[i].parent];
                    animatedJoint0.position = parentJoint0.position + parentJoint0.orientation * animatedJoint0.position;
                    animatedJoint0.orientation = glm::normalize(parentJoint0.orientation * animatedJoint0.orientation);

                    Resources::Md5Model::FrameJoint const& parentJoint1 = parents1[joints[i].parent];
                    animatedJoint1.position = parentJoint1.position + parentJoint1.orientation * animatedJoint1.position;
                    animatedJoint1.orientation = glm::normalize(parentJoint1.orientation * animatedJoint1.orientation);
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
                    float pi = std::atan2(0.0f, -1.0f);
                    animatedJoint0.orientation = glm::normalize(glm::angleAxis<float>(glm::degrees(phi - pi/2)/4, 1, 0, 0) * animatedJoint0.orientation);
                    animatedJoint1.orientation = glm::normalize(glm::angleAxis<float>(glm::degrees(phi - pi/2)/4, 1, 0, 0) * animatedJoint1.orientation);
                }

                this->_animatedBones[i] =
                    (
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
            std::vector<Resources::Md5Model::FrameJoint> parents(joints.size());

            for (unsigned int i = 0; i < joints.size(); ++i)
            {
                Resources::Md5Model::FrameJoint& animatedJoint = parents[i];
                animatedJoint = Resources::Md5Model::FrameJoint(joints[i].position, joints[i].orientation, joints[i].size);

                if (joints[i].parent >= 0) // Has a parent joint
                {
                    Resources::Md5Model::FrameJoint const& parentJoint = parents[joints[i].parent];
                    animatedJoint.position = parentJoint.position + parentJoint.orientation * (animatedJoint.position/* - joints[joints[i].parent].position*/);
                    animatedJoint.orientation = glm::normalize(parentJoint.orientation * animatedJoint.orientation);
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
                    float pi = std::atan2(0.0f, -1.0f);
                    animatedJoint.orientation =
                        glm::normalize(
                                glm::angleAxis<float>(glm::degrees(phi - pi/2)/4, 1, 0, 0)
                                *
                                animatedJoint.orientation
                                );
                }

                this->_animatedBones[i] =
                    (
                     glm::translate(animatedJoint.position)
                     *
                     glm::toMat4(animatedJoint.orientation)
                    )
                    *
                    this->_model.GetInverseBindPose()[i]
                    ;

                //animatedJoint.orientation = glm::quat();

                //if (joints[i].parent >= 0) // Has a parent joint
                //{
                //    Resources::Md5Model::FrameJoint const& parentJoint = parents[joints[i].parent];
                //    animatedJoint.orientation = glm::normalize(parentJoint.orientation * animatedJoint.orientation);
                //}

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
                //    float pi = std::atan2(0.0f, -1.0f);
                //    animatedJoint.orientation = glm::normalize(glm::angleAxis<float>(glm::degrees(phi - pi/2)/4, 1, 0, 0) * animatedJoint.orientation);
                //}

            }
        }
    }

}}
