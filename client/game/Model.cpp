#include "client/game/Model.hpp"

#include "client/resources/Md5Animation.hpp"
#include "client/resources/LocalResourceManager.hpp"

namespace Client { namespace Game {

    Model::Model(Resources::LocalResourceManager& resourceManager) :
        _model(resourceManager.GetMd5Model("boblampclean")),
        _curAnimation(),
        _animatedBones(_model.GetNumJoints()),
        _animTime(0)
    {
        this->_animations["boblampclean"] = &resourceManager.GetMd5Animation("boblampclean");
        if (!this->_model.CheckAnimation(*this->_animations["boblampclean"]))
            throw std::runtime_error("this animation is not compatible");
        this->_animations["boblampclean_move"] = &resourceManager.GetMd5Animation("boblampclean_move");
        if (!this->_model.CheckAnimation(*this->_animations["boblampclean_move"]))
            throw std::runtime_error("this animation is not compatible");
        this->_curAnimation = this->_animations["boblampclean_move"];
    }

    void Model::Update(Uint32 time, float phi)
    {
        if (this->_curAnimation != 0 && this->_curAnimation->GetNumFrames() > 0)
        {
            float deltaTime = (float)time / 1000.0f;

            Resources::Md5Animation const& animation = *this->_curAnimation;

            this->_animTime = std::fmod(this->_animTime + deltaTime, animation.GetAnimDuration());

            // Figure out which frame we're on
            float framNum = this->_animTime * (float)animation.GetFramRate();
            int frame0 = (int)std::floor(framNum) % animation.GetNumFrames();
            int frame1 = (int)std::ceil(framNum) % animation.GetNumFrames();

            float interpolate = std::fmod(this->_animTime, animation.GetFrameDuration()) / animation.GetFrameDuration();

            Resources::Md5Animation::FrameSkeleton const& skeleton0 = animation.GetSkeleton(frame0);
            Resources::Md5Animation::FrameSkeleton const& skeleton1 = animation.GetSkeleton(frame1);
            std::vector<Resources::Md5Animation::SkeletonJoint> parents0(this->_model.GetNumJoints());
            std::vector<Resources::Md5Animation::SkeletonJoint> parents1(this->_model.GetNumJoints());

            for (unsigned int i = 0; i < this->_model.GetNumJoints(); ++i)
            {
                Resources::Md5Animation::SkeletonJoint& animatedJoint0 = parents0[i];
                animatedJoint0 = skeleton0.joints[i];
                Resources::Md5Animation::SkeletonJoint& animatedJoint1 = parents1[i];
                animatedJoint1 = skeleton1.joints[i];

                if (animatedJoint0.parent >= 0) // Has a parent joint
                {
                    Resources::Md5Animation::SkeletonJoint const& parentJoint0 = parents0[animatedJoint0.parent];
                    animatedJoint0.pos = parentJoint0.pos + parentJoint0.orient * animatedJoint0.pos;
                    animatedJoint0.orient = glm::normalize(parentJoint0.orient * animatedJoint0.orient);

                    Resources::Md5Animation::SkeletonJoint const& parentJoint1 = parents1[animatedJoint1.parent];
                    animatedJoint1.pos = parentJoint1.pos + parentJoint1.orient * animatedJoint1.pos;
                    animatedJoint1.orient = glm::normalize(parentJoint1.orient * animatedJoint1.orient);
                }

                if (
                        animation.GetJointInfo(i).name == "pelvis"
                        ||
                        animation.GetJointInfo(i).name == "neck"
                        ||
                        animation.GetJointInfo(i).name == "head"
                        ||
                        animation.GetJointInfo(i).name == "spine"
                   )
                {
                    float pi = std::atan2(0.0f, -1.0f);
                    animatedJoint0.orient = glm::normalize(glm::angleAxis<float>(glm::degrees(phi - pi/2)/4, 1, 0, 0) * animatedJoint0.orient);
                    animatedJoint1.orient = glm::normalize(glm::angleAxis<float>(glm::degrees(phi - pi/2)/4, 1, 0, 0) * animatedJoint1.orient);
                }

                this->_animatedBones[i] =
                    (
                     glm::translate(
                         glm::lerp(animatedJoint0.pos, animatedJoint1.pos, interpolate) // finalPos
                         )
                     *
                     glm::toMat4(
                         glm::shortMix(animatedJoint0.orient, animatedJoint1.orient, interpolate) // finalOrient
                         )
                    )
                    *
                    this->_model.GetInverseBindPose()[i]
                    ;
            }

        }
        else
        {
            std::vector<Resources::Md5Model::Joint> parents(this->_model.GetNumJoints());

            for (unsigned int i = 0; i < this->_model.GetNumJoints(); ++i)
            {
                Resources::Md5Model::Joint& animatedJoint = parents[i];
                animatedJoint = this->_model.GetJoints()[i];

                if (animatedJoint.parentID >= 0) // Has a parent joint
                {
                    Resources::Md5Model::Joint const& parentJoint = parents[animatedJoint.parentID];
                    animatedJoint.pos = parentJoint.pos + parentJoint.orient * (animatedJoint.pos - this->_model.GetJoints()[animatedJoint.parentID].pos);
                    animatedJoint.orient = glm::normalize(parentJoint.orient * animatedJoint.orient);
                }

                if (
                        animatedJoint.name == "pelvis"
                        ||
                        animatedJoint.name == "neck"
                        ||
                        animatedJoint.name == "head"
                        ||
                        animatedJoint.name == "spine"
                   )
                {
                    float pi = std::atan2(0.0f, -1.0f);
                    animatedJoint.orient =
                        glm::normalize(
                                glm::angleAxis<float>(glm::degrees(phi - pi/2)/4, 1, 0, 0)
                                *
                                animatedJoint.orient
                                );
                }

                this->_animatedBones[i] =
                    (
                     glm::translate(animatedJoint.pos)
                     *
                     glm::toMat4(animatedJoint.orient)
                    )
                    *
                    this->_model.GetInverseBindPose()[i]
                    ;

                animatedJoint.orient = glm::quat();

                if (animatedJoint.parentID >= 0) // Has a parent joint
                {
                    Resources::Md5Model::Joint const& parentJoint = parents[animatedJoint.parentID];
                    animatedJoint.orient = glm::normalize(parentJoint.orient * animatedJoint.orient);
                }

                if (
                        animatedJoint.name == "pelvis"
                        ||
                        animatedJoint.name == "neck"
                        ||
                        animatedJoint.name == "head"
                        ||
                        animatedJoint.name == "spine"
                   )
                {
                    float pi = std::atan2(0.0f, -1.0f);
                    animatedJoint.orient = glm::normalize(glm::angleAxis<float>(glm::degrees(phi - pi/2)/4, 1, 0, 0) * animatedJoint.orient);
                }

            }

        }
    }

}}
