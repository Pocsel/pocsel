#include "client/game/Model.hpp"

namespace Client { namespace Game {

    Model::Model(Resources::Md5Model const& model) :
        _model(model);
    {
    }

    void Model::Update(Uint32 time, float phi)
    {
        if (this->_model.HasAnimation())
        {
            float deltaTime = (float)time / 1000.0f;

            this->_UpdateAnimation(deltaTime, phi);

            std::vector<glm::mat4x4> const& animatedSkeleton = this->_animation.GetSkeletonMatrixList();

            // Multiply the animated skeleton joints by the inverse of the bind pose.
            for (int i = 0; i < this->_numJoints; ++i)
            {
                this->_animatedBones[i] = animatedSkeleton[i] * this->_inverseBindPose[i];
            }
        }
    }

    void Model::_UpdateAnimation(float deltaTime, float phi)
    {
        if (this->_numFrames < 1)
            return;

        this->_animTime += deltaTime;

        while (this->_animTime > this->_animDuration)
            this->_animTime -= this->_animDuration;
        while (this->_animTime < 0.0f)
            this->_animTime += this->_animDuration;

        // Figure out which frame we're on
        float framNum = this->_animTime * (float)this->_framRate;
        int frame0 = (int)std::floor(framNum);
        int frame1 = (int)std::ceil(framNum);
        frame0 = frame0 % this->_numFrames;
        frame1 = frame1 % this->_numFrames;

        float interpolate = std::fmod(this->_animTime, this->_frameDuration ) / this->_frameDuration;

        FrameSkeleton& skeleton0 = this->_skeletons[frame0];
        FrameSkeleton& skeleton1 = this->_skeletons[frame1];
        std::vector<SkeletonJoint> parents0;
        std::vector<SkeletonJoint> parents1;
        parents0.reserve(this->_numJoints);
        parents1.reserve(this->_numJoints);
        for (int i = 0; i < this->_numJoints; ++i)
        {
            SkeletonJoint& animatedJoint0 = parents0[i];
            animatedJoint0 = skeleton0.joints[i];
            if (animatedJoint0.parent >= 0) // Has a parent joint
            {
                SkeletonJoint const& parentJoint = parents0[animatedJoint0.parent];
                glm::vec3 rotPos = parentJoint.orient * animatedJoint0.pos;
                animatedJoint0.pos = parentJoint.pos + rotPos;
                animatedJoint0.orient = parentJoint.orient * animatedJoint0.orient;
                animatedJoint0.orient = glm::normalize(animatedJoint0.orient);
            }
            SkeletonJoint& animatedJoint1 = parents1[i];
            animatedJoint1 = skeleton1.joints[i];
            if (animatedJoint1.parent >= 0) // Has a parent joint
            {
                SkeletonJoint const& parentJoint = parents1[animatedJoint1.parent];
                glm::vec3 rotPos = parentJoint.orient * animatedJoint1.pos;
                animatedJoint1.pos = parentJoint.pos + rotPos;
                animatedJoint1.orient = parentJoint.orient * animatedJoint1.orient;
                animatedJoint1.orient = glm::normalize(animatedJoint1.orient);
            }

            if (
                   this->_jointInfos[i].name == "pelvis"
                   ||
                   this->_jointInfos[i].name == "neck"
                   ||
                   this->_jointInfos[i].name == "head"
                   ||
                   this->_jointInfos[i].name == "spine"
               )
            {
                float pi = std::atan2(0.0f, -1.0f);
                animatedJoint0.orient = glm::normalize(glm::angleAxis<float>(glm::degrees(phi - pi/2)/4, 1, 0, 0) * animatedJoint0.orient);
                animatedJoint1.orient = glm::normalize(glm::angleAxis<float>(glm::degrees(phi - pi/2)/4, 1, 0, 0) * animatedJoint1.orient);
            }
            SkeletonJoint& finalJoint = finalSkeleton.joints[i];
            glm::mat4x4& finalMatrix = finalSkeleton.boneMatrices[i];


            finalPos = glm::lerp(animatedJoint0.pos, animatedJoint1.pos, interpolate);
            finalOrient = glm::shortMix(animatedJoint0.orient, animatedJoint1.orient, interpolate);

            finalMatrix = glm::translate(finalJoint.pos) * glm::toMat4(finalJoint.orient);
        }
    }

}}
